# `sigzip` - A set of utilities to embed signatures into gzips
WIP. 

```
cc -o extract extract.c
./extract test.tar.gz >> output.test.sig
diff --report-identical-files output.test.sig test.tar.sig
gunzip test.tar.gz
gpg --verify output.test.sig
```

TODO: injecting signatures.
