call "" amd64
cl /Zc:wchar_t /GS /GL /W3 /O2 /Zc:inline /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_WINDLL" /Oy /Gd /Gy /Oi /GR- /GF /Ot /fp:precise /MT /std:c++17 /LD /EHa /WX- /nologo "C:\SierraChart\ACS_Source\A.cpp"  /link "Gdi32.lib" "User32.lib" /DLL /DYNAMICBASE /INCREMENTAL:NO /OPT:REF /OPT:ICF /MACHINE:X64 /OUT:"C:\SierraChart\Data\A_64.dll"
