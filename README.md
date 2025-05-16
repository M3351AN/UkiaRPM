it's a template/base to make a simple hack

currently is a simple esp hack for CS:S steam x64

since this repo was private for some days. commit history maybe messd up
## Feature 
- Use Native APIs to OpenProcess/WPM/RPM/CreateWindow
- Multithread
- Basic imgui render
- overlay at most top with uiaccess
- automatic change exe file hash eachtime run/exit
## To use it
- modify those offsets and struct to game u wanna hack(current struct is completely irregular, only for testing)
- modify functions that u wanna make
- if u wanna write memory, add vm write flag in attach() func
- if possible, use kernel RPM/WPM like aimstar ring0
## Credits
- Zebra for his CS1.6 hack Evelion
- Liv,CowNow,Me and any others who ever contribute to Aimstar
- Friendly users on UC that ever helps me
