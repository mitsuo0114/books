This is based on following book
  https://www.sigbus.info/compilerbook/

# memo

New line code replace
```
git grep -l -I $'\r$' | xargs -d '\n' nkf -Lu --overwrite

```

