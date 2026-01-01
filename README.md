# Typy algebraiczne C++

To repozytorium zawiera biblioteczne rozwiązanie problemu braku pattern matchingu w C++. Wprowadza funkcję `Inspect`, która może być używana w sposób podobny do instrukcji `switch`, z tym, że nie jest to instrukcja a wyrażenie (`switch` nie może zwrócić wartości). Dodano także własną implementację `std::expected`, która działa na C++17. Funkcja `Inspect` używa też bardziej zrozumiałego logowania błędów (mówi, że zapomniano zdefiniować obsługi przypadku).

## Uruchomienie

Zapewniono dwa sposoby budowania, jeden używa narzędzia Meson, czemu? Żeby zobaczyć jak się z tego korzysta, a nie wieczne klepanie CMake, albo co gorsza Make. Dla wygody zapewniono Justfile (taki make, tylko że jest to uruchamiacz zadań, a nie system budowania).

### Konfiguracja _Meson_

Należy mieć zainstalowany _Meson_ oraz _just_ (do Justfile). Aby uruchomić, wykonaj:
```bash
just
```

To wszystko.

### Konfiguracja _Makefile_

Ten projekt jest prosty, więc zrobienie _makefile'a_ nie wymagało wiele zachodu. Wybrano konfigurację z kompilatoremm _LLVM_, bo jest trochę bardziej przenośny niż _GCC_. Zatem, trzeba mieć _make_ oraz _clang++_ do samej kompilacji.

**UWAGA**: modyfikując `Makefile` można odkomentowując `# CXX=g++` zmienić kompilator na GCC.

Aby uruchomić program wykonaj komendę:
```bash
make
```
To wszystko.