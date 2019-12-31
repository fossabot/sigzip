#!/usr/bin/env bash
set -euo pipefail
echo "--- MAKE ---"
make all
echo "--- GENERATE TEST FILES ---"
tar -cvf testrun.tar LICENSE
shasum -a 256 testrun.tar > testrun.sha256sum
gzip testrun.tar
echo "--- INJECT HASH ---"
./inject testrun.tar.gz testrun.sha256sum > testrun_sha.tar.gz
rm testrun.sha256sum
echo "--- EXTRACT HASH ---"
./extract testrun_sha.tar.gz > testrun_extracted.sha256sum
echo "--- GUNZIP INJECTED ---"
gunzip -kc testrun_sha.tar.gz > testrun.tar
echo "--- VALIDATE ---"
shasum -a 256 testrun.tar > testrun.sha256sum
diff -s testrun_extracted.sha256sum testrun.sha256sum
echo "--- CLEANUP ---"
rm testrun*
make clean