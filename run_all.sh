comp
time cmsRun test/PrimitivesPhase2Prod_cfg.py > $T/output.txt
cd html
root -b make_up.C
./update.sh
