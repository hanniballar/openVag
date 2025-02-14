const char * getOpenVagDefaultIni() {
    return R"openVag([Window][WindowOverViewport_11111111]
Pos=0,19
Size=1264,742
Collapsed=0

[Window][Debug##Default]
Pos=316,145
Size=400,400
Collapsed=0

[Window][Dear ImGui Metrics/Debugger]
Pos=974,648
Size=290,113
Collapsed=0
DockId=0x00000005,0

[Window][Find]
Pos=974,19
Size=290,305
Collapsed=0
DockId=0x00000006,0

[Window][Canvas]
Pos=0,19
Size=972,742
Collapsed=0
DockId=0x00000001,0

[Window][Edge properties]
Pos=1345,19
Size=575,953
Collapsed=0
DockId=0x00000007,1

[Window][properties]
Pos=826,19
Size=438,708
Collapsed=0
DockId=0x00000007,1

[Window][Properties]
Pos=974,326
Size=290,320
Collapsed=0
DockId=0x00000007,0

[Window][Config]
Pos=985,382
Size=279,264
Collapsed=0
DockId=0x00000007,1

[Window][About Dear ImGui]
Pos=60,60
Size=325,48
Collapsed=0

[Window][About openVag]
Pos=220,79
Size=325,48
Collapsed=0

[Window][Open File##OpenFileDlgKey]
Pos=60,60
Size=716,284
Collapsed=0

[Window][Save File##SaveFileDlgKey]
Pos=60,60
Size=488,285
Collapsed=0

[Window][Validation messages]
Pos=1630,748
Size=290,269
Collapsed=0
DockId=0x00000005,1
[Table][0xD09F7F78,4]
RefScale=13
Column 0  Sort=0v

[Docking][Data]
DockSpace       ID=0x7C6B3D9B Window=0xA87D555D Pos=108,150 Size=1264,742 Split=X Selected=0x429E880E
  DockNode      ID=0x00000001 Parent=0x7C6B3D9B SizeRef=1014,998 CentralNode=1 HiddenTabBar=1 Selected=0x429E880E
  DockNode      ID=0x00000002 Parent=0x7C6B3D9B SizeRef=290,998 Split=Y Selected=0xD8064768
    DockNode    ID=0x00000003 Parent=0x00000002 SizeRef=322,627 Split=Y Selected=0x62307AD6
      DockNode  ID=0x00000006 Parent=0x00000003 SizeRef=279,305 HiddenTabBar=1 Selected=0x62307AD6
      DockNode  ID=0x00000007 Parent=0x00000003 SizeRef=279,320 HiddenTabBar=1 Selected=0x94B17C7D
    DockNode    ID=0x00000005 Parent=0x00000002 SizeRef=322,113 HiddenTabBar=1 Selected=0x20E3C5EA

[LayerTypeColor][Data]
Const=(0.220000, 0.220000, 0.220000, 0.900000)
Parameter=(0.135000, 0.155000, 0.135000, 0.784314)
Result=(0.135000, 0.135000, 0.155000, 0.784314)
)openVag";
}