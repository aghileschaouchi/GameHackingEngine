# What is it?
## This is the C/C++ simple project version of the Windows GameHackingEngine (GHE), soon it will be realesed as a .dll also. 
## It's intended to be a useful generic game independent engine used to build game hacks and that can be easily integrated into a GUI or a web application via Emscripten.

## Features to add:
1. an AddressClass
2. a ProcessManager class using the address class
3. a utils namespace: should be handling Windows to cpp types and vis versa transformations such as TCHAR* to std::string
4. a GameInfoManager class, data should be serialized and deserialized to be read by the GameInfoManager class, thus there should be independent serializer
5. Serializers/Deserializers

## Prerequisite:
1. add kernel32.lib to the linker's inputs
2. install and add msctf.lib 