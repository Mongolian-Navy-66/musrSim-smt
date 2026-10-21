# Git 收录与提交检查

本仓库独立维护现有历史、分支与远程。源码、测试、构建配置、说明文档和维护中的运行模板使用普通 Git 跟踪：新增文件可直接出现在待审列表，无须登记整库文件清单。

## 成果及数据

原始数据、批量生成的 macro、逐事件输出、缓存、日志和构建产物保留本地。正式报告、配图、精选结果、样例和特殊输入使用精确白名单。

- `manifests/publication_policy.json`：数据类型、产物目录及单文件 10 MiB 上限。
- `manifests/publication_assets.txt`：获准成果与特殊输入的仓库相对路径。只列实际文件，不能用目录或通配符扩大范围。
- `manifests/publication_link_exceptions.json`：逐条审核的历史缺失、仓库外引用和模板占位符。新增缺失不会自动获准；例外过期会报错。

保留仿真源码、测试、第三方代码及许可证、运行模板和必要校准输入。现有 `run/run_many_files.tar.gz` 是明确保留的历史运行模板包；新增归档包仍需单独列入白名单。

## 日常流程

普通源码按具体路径暂存，然后检查。新增成果需先审核其大小、来源及报告依赖，补充白名单，再生成忽略规则：

```bash
python tools/check_publication.py --write-ignore
```

该命令只从工作区的策略与白名单更新根目录 `.gitignore`，不自动暂存。使用 `git add -- <具体文件路径>` 明确暂存源码或获准成果，以及本次修改的规则、白名单和说明。避免批量强制添加结果目录。

```bash
python tools/check_publication.py --index
git diff --cached --stat
git diff --cached --check
```

检查器默认读取暂存区 blob，包括暂存的策略、白名单和文档；本地未暂存的依赖文件不能使检查通过。它检查文件类型和大小、Python 语法、Markdown/TeX 文件依赖及明确的历史例外。它不验证物理结论，也不替代业务测试。仅已获准的小型 pilot ROOT 会做内容哈希，生产 ROOT/DAT 不解析。

从暂存内容导出完整的独立目录后，可用以下命令检查；目录中应只包含拟发布文件：

```bash
python tools/check_publication.py --tree /path/to/clean-export
```

`--audit-links` 只列出待审依赖，不会更新例外。报告模板中的占位符只在对应文档和目标的明确例外中获准。历史外部引用不代表其资源已随仓库发布。

检查工具自身的回归测试：`python tools/test_publication.py`。测试使用临时仓库和临时索引，不修改本仓库的暂存内容。

## 跨平台约定

本地仓库设置 `core.filemode=false`，避免 Windows/SSHFS 的执行权限误报；Git 索引中的原有脚本执行位保留。以后有意修改执行位时使用 `git update-index --chmod=+x -- <脚本>` 并审核该变化。

二进制附件保持原始字节，shell 脚本使用 LF；冻结证据沿用原属性。新增规则不触发全库换行重写。没有配置自动提交或推送 hook；检查通过后由维护者自行 commit/push。

数据可用性见 [DATA_AVAILABILITY.md](DATA_AVAILABILITY.md)。本仓库的提交检查只依赖 Python 标准库、Git 和仓库内文件，无须工作区审计目录。
