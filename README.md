it's a template/base to make a simple hack

currently is a simple esp hack for CS:S steam x64

since this repo was private for some days. commit history maybe messd up
## Feature 
- Use Native APIs to OpenProcess/WPM/RPM/CreateWindow
- Multithread
- Basic imgui render
- overlay at most top with uiaccess(which allows you overlay while game in full-screen mode, but some bugs need to resolve)
- automatic change exe file hash eachtime run/exit
- Basic config system with yaml-cpp
## To use it
- modify those offsets and struct to game u wanna hack(current struct is completely irregular, only for testing)
- modify functions that u wanna make
- if u wanna write memory, add vm write flag in attach() func
- if possible, use kernel RPM/WPM like aimstar ring0
- remember,"The security of an entire system is only equal to the security of its weakest link.
"
## Credits
- Zebra for his CS1.6 hack Evelion
- Liv,CowNow,Me and any others who ever contribute to Aimstar
- Friendly users on UC that ever helps me
- DeepSeek R1 for cleaning my sh1t code
