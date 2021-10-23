# GentooUpdateNotifier
A little utility to look for updates on Gentoo written in C++

# How to compile
```
make -j$(nproc)
```

If you want this program to synchronize for you, you'll need to compile it for that
```
make -j$(nproc) sync
```
You need to run this as a user root, dah!

# Example
```
sudo ./GentooUpdateNotifier 7 d
```
It will synchronize every 7 days and tell you how many updates you have.

# Enjoy!
