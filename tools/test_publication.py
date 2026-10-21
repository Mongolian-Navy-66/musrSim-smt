"""Run with python tools/test_publication.py; all fixtures use temporary repositories."""
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest

import check_publication as checker


class PublicationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='publication-test-')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.run_git('init', '-q')
        self.run_git('config', 'core.autocrlf', 'false')
        self.run_git('config', 'core.filemode', 'false')
        self.policy = {
            'schema_version': 1, 'max_file_bytes': 10485760,
            'asset_suffixes': ['.root', '.dat', '.png'],
            'local_filenames': ['*.pyc'], 'local_directories': ['build', '__pycache__'],
            'protected_prefixes': ['reports/', 'results/'],
            'simulation_generated_directories': ['macs*', 'data*', 'pilot', 'validation*', 'production'],
            'local_paths': ['simulation/sim7/production.sub'],
        }
        self.assets = set()
        self.write(checker.EXCEPTIONS, '[]\n')
        self.rules()
        self.run_git('add', '--', '.gitignore', checker.POLICY, checker.ASSETS, checker.EXCEPTIONS)

    def run_git(self, *args, **kwargs):
        return subprocess.run(['git', '-C', str(self.root), *args], check=True,
                              stdout=subprocess.PIPE, stderr=subprocess.PIPE, **kwargs)

    def write(self, name, text):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding='utf-8')

    def stage(self, name, text):
        self.write(name, text)
        self.run_git('add', '-f', '--', name)

    def rules(self):
        self.write(checker.POLICY, json.dumps(self.policy))
        self.write(checker.ASSETS, ''.join(x + '\n' for x in sorted(self.assets)))
        self.write('.gitignore', checker.ignore_text(self.policy, self.assets))

    def validate(self):
        return checker.validate(checker.Snapshot(self.root, True))

    def ignored(self, path):
        return subprocess.run(['git', '-C', str(self.root), 'check-ignore', '-q', '--no-index', '--', path]).returncode == 0

    def test_new_source_needs_no_manifest_entry(self):
        self.assertFalse(self.ignored('src/new.py'))
        self.assertFalse(self.ignored('simulation/sim7/new.mac'))
        self.stage('src/new.py', 'value = 1\n')
        self.assertEqual(self.validate()['allowlisted_artifacts'], 0)

    def test_generated_paths_and_forced_raw_data(self):
        for path in ['data.root', 'reports/unapproved.md', 'build/output',
                     'simulation/sim4/macs_plane_truth/000.mac', 'simulation/sim6/validation2/pilot.mac',
                     'simulation/sim7/production/000.mac', 'simulation/sim7/production.sub']:
            self.assertTrue(self.ignored(path), path)
        self.stage('data.root', 'not a ROOT payload\n')
        with self.assertRaisesRegex(ValueError, 'not allowlisted'):
            self.validate()

    def test_unapproved_and_approved_attachment(self):
        self.stage('reports/figure.png', 'fixture bytes\n')
        with self.assertRaisesRegex(ValueError, 'not allowlisted'):
            self.validate()
        self.assets.add('reports/figure.png')
        self.rules()
        self.run_git('add', '--', checker.ASSETS, '.gitignore')
        self.assertFalse(self.ignored('reports/figure.png'))
        self.assertTrue(self.ignored('reports/another.png'))
        self.assertEqual(self.validate()['allowlisted_artifacts'], 1)

    def test_worktree_dependency_cannot_hide_missing_staged_file(self):
        self.stage('README.md', '[input](input.md)\n')
        self.write('input.md', 'Not staged\n')
        with self.assertRaisesRegex(ValueError, 'Unapproved missing'):
            self.validate()
        self.run_git('add', '--', 'input.md')
        self.validate()

    def test_staged_document_is_checked_in_both_directions(self):
        self.stage('README.md', '[bad](absent.md)\n')
        self.write('README.md', 'Working tree correction only\n')
        with self.assertRaisesRegex(ValueError, 'Unapproved missing'):
            self.validate()
        self.run_git('add', '--', 'README.md')
        self.write('README.md', '[bad](another-absent.md)\n')
        self.validate()

    def test_unstaged_policy_cannot_grant_permission(self):
        self.stage('raw.root', 'fixture\n')
        self.assets.add('raw.root')
        self.rules()
        with self.assertRaisesRegex(ValueError, 'not allowlisted'):
            self.validate()

    def test_new_external_and_template_references_require_review(self):
        self.stage('README.md', '[external](../other/report.md)\n[template]({{ASSET}}/plot.png)\n')
        with self.assertRaisesRegex(ValueError, 'Unapproved external'):
            self.validate()

    def test_existing_exception_and_staleness(self):
        self.stage('README.md', '[historical](missing.md)\n')
        self.stage(checker.EXCEPTIONS, json.dumps([{
            'document': 'README.md', 'target': 'missing.md', 'kind': 'missing',
            'reason': 'Historical fixture',
        }]))
        self.validate()
        self.stage('missing.md', 'Now available\n')
        with self.assertRaisesRegex(ValueError, 'Stale missing'):
            self.validate()

    def test_reference_links_and_tex_dependencies(self):
        self.stage('README.md', '![figure][f]\n\n[f]: missing.png\n')
        with self.assertRaisesRegex(ValueError, 'Unapproved missing'):
            self.validate()
        self.stage('README.md', 'Clean\n')
        self.stage('main.tex', '\\input{missing-section}\n')
        with self.assertRaisesRegex(ValueError, 'Unapproved missing'):
            self.validate()

    def test_size_and_rule_drift(self):
        self.policy['max_file_bytes'] = 2048
        self.rules()
        self.run_git('add', '--', checker.POLICY)
        self.stage('large.txt', 'x' * 2049)
        with self.assertRaisesRegex(ValueError, 'exceeds size limit'):
            self.validate()
        self.run_git('rm', '--cached', '--', 'large.txt')
        self.stage('.gitignore', 'incorrect rules\n')
        with self.assertRaisesRegex(ValueError, 'Ignore rules differ'):
            self.validate()

    def test_isolated_index_and_clean_export(self):
        original_index = (self.root / '.git/index').read_bytes()
        with tempfile.TemporaryDirectory(prefix='publication-index-') as index_dir:
            index = Path(index_dir) / 'index'
            index.write_bytes(original_index)
            old = os.environ.get('GIT_INDEX_FILE')
            os.environ['GIT_INDEX_FILE'] = str(index)
            try:
                self.stage('new.py', 'value = 2\n')
                snapshot = checker.Snapshot(self.root, True)
                checked = checker.validate(snapshot)
                with tempfile.TemporaryDirectory(prefix='publication-export-') as export_dir:
                    export = Path(export_dir)
                    for name in snapshot.entries:
                        path = export / name
                        path.parent.mkdir(parents=True, exist_ok=True)
                        path.write_bytes(snapshot.read(name))
                    exported = checker.validate(checker.Snapshot(export, False))
                    self.assertEqual(checked['files'], exported['files'])
            finally:
                if old is None:
                    del os.environ['GIT_INDEX_FILE']
                else:
                    os.environ['GIT_INDEX_FILE'] = old
        self.assertEqual((self.root / '.git/index').read_bytes(), original_index)


if __name__ == '__main__':
    unittest.main()
