#!/usr/bin/env python3
"""Compare t1 values for matched ROI-off/on Geant4 pilot runs (requires PyROOT)."""
import argparse
import math

def same(a,b):
    return a == b or (isinstance(a,float) and isinstance(b,float) and math.isnan(a) and math.isnan(b))

def open_root(path):
    import ROOT
    file=ROOT.TFile.Open(path)
    if not file or file.IsZombie():
        raise ValueError("cannot open "+path)
    return file

def compare(off_path,on_path):
    off,on=open_root(off_path),open_root(on_path)
    try:
        a,b=off.Get("t1"),on.Get("t1")
        truth=on.Get("roiTruth")
        if not a or not b or not truth:
            raise ValueError("both files need t1; ROI-on file needs roiTruth")
        if off.Get("roiTruth"):
            raise ValueError("ROI-off file unexpectedly contains roiTruth")
        names_a=[x.GetName() for x in a.GetListOfBranches()]
        names_b=[x.GetName() for x in b.GetListOfBranches()]
        if names_a != names_b or a.GetEntries()!=b.GetEntries():
            raise AssertionError("t1 schemas or event counts differ")
        leaves_a=list(a.GetListOfLeaves()); leaves_b=list(b.GetListOfLeaves())
        signature=lambda leaves:[(v.GetName(),v.GetTypeName()) for v in leaves]
        if signature(leaves_a)!=signature(leaves_b):
            raise AssertionError("t1 leaf schemas differ")
        stored=set()
        for i in range(int(a.GetEntries())):
            a.GetEntry(i); b.GetEntry(i)
            stored.add((int(b.runID),int(b.eventID)))
            for x,y in zip(leaves_a,leaves_b):
                if x.GetLen()!=y.GetLen():
                    raise AssertionError(f"entry {i} leaf {x.GetName()} lengths differ")
                for j in range(x.GetLen()):
                    if not same(x.GetValue(j),y.GetValue(j)):
                        raise AssertionError(f"entry {i} leaf {x.GetName()}[{j}] differs")
        generated=int(round(on.Get("geantParametersD")[5]))
        if truth.GetEntries()!=generated:
            raise AssertionError("roiTruth entries differ from generated event count")
        truth_stored=set()
        for event in truth:
            if event.stored: truth_stored.add((int(event.runID),int(event.eventID)))
            if not (len(event.roiName)==len(event.muonEntered)==len(event.muonEdep)==len(event.muonDeposit)):
                raise AssertionError("ROI branch lengths differ")
            for energy,flag in zip(event.muonEdep,event.muonDeposit):
                if bool(flag)!=(float(energy)>0):
                    raise AssertionError("muon Edep and deposit flag differ")
        if stored!=truth_stored:
            raise AssertionError("roiTruth stored keys differ from t1")
        print(f"matched t1: {a.GetEntries()} events, {len(leaves_a)} leaves; roiTruth: {generated} events")
    finally:
        off.Close(); on.Close()

if __name__=="__main__":
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("roi_off")
    parser.add_argument("roi_on")
    args=parser.parse_args()
    compare(args.roi_off,args.roi_on)
