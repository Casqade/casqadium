#include <cqde/static_strings.hpp>


namespace cqde
{

const char* MemoryResidentPath()
{
  return "***memory***";
}

const char* EditorImGuiSettings()
{
  return
R"ini(
[Window][Assets]
Pos=0,191
Size=199,466
Collapsed=0
DockId=0x00000011,1

[Window][Callbacks]
Size=241,189
Collapsed=0
DockId=0x00000002,1

[Window][Registry view]
Pos=1257,0
Size=343,435
Collapsed=0
DockId=0x0000000A,0

[Window][Component view]
Pos=1257,437
Size=343,212
Collapsed=0
DockId=0x00000015,0

[Window][Input]
Pos=0,191
Size=199,466
Collapsed=0
DockId=0x00000011,2

[Window][Systems]
Pos=0,0
Size=199,189
Collapsed=0
DockId=0x00000002,0

[Window][Viewports]
Pos=0,0
Size=199,189
Collapsed=0
DockId=0x00000002,1

[Window][Info]
Pos=400,5
Size=135,48
Collapsed=0

[Window][InputBinding]
Pos=0,659
Size=199,241
Collapsed=0
DockId=0x00000012,0

[Window][PackageEdit]
Pos=0,541
Size=241,359
Collapsed=0
DockId=0x0000000E,0

[Window][Scene graph view]
Pos=1256,0
Size=154,435
Collapsed=0
DockId=0x00000009,0

[Window][Debug##Default]
Pos=60,60
Size=400,400
Collapsed=0

[Window][###axisEditWindow]
Pos=1257,632
Size=343,268
Collapsed=0
DockId=0x0000000C,0

[Window][###assetEditWindow]
Pos=0,549
Size=199,351
Collapsed=0
DockId=0x00000010,0

[Window][DockSpaceViewport_11111111]
Pos=0,0
Size=1600,900
Collapsed=0

[Window][Packages]
Pos=0,191
Size=199,466
Collapsed=0
DockId=0x00000011,0

[Window][Viewport 0##viewport]
Pos=201,0
Size=1054,900
Collapsed=0
DockId=0x00000013,0

[Window][Viewport 1##viewport]
Pos=243,594
Size=1015,306
Collapsed=0
DockId=0x00000014,0

[Window][##guizmoCube]
Pos=60,60
Size=32,35
Collapsed=0

[Window][Prefabs]
Pos=1257,437
Size=343,212
Collapsed=0
DockId=0x00000015,1

[Window][###colliderEditWindow]
Pos=1257,630
Size=343,270
Collapsed=0
DockId=0x00000018,0

[Window][###stepEditWindow]
Pos=1257,636
Size=343,264
Collapsed=0
DockId=0x0000001A,0

[Window][###audioGroupEditWindow]
Pos=1257,651
Size=343,249
Collapsed=0
DockId=0x0000001C,0

[Window][###actionEditWindow]
Pos=1257,671
Size=343,229
Collapsed=0
DockId=0x00000016,0

[Table][0xF1FE4FE2,2]
Column 0  Weight=0.9686
Column 1  Weight=1.0314

[Table][0x170B897A,2]
Column 0  Weight=0.9630
Column 1  Weight=1.0370

[Table][0x1BB3D1B1,4]
Column 0  Weight=0.3684
Column 1  Weight=1.2105
Column 2  Weight=1.2105
Column 3  Weight=1.2105

[Table][0x3B762056,4]
Column 0  Weight=0.3373
Column 1  Weight=1.2048
Column 2  Weight=1.2048
Column 3  Weight=1.2530

[Table][0x42E783F6,4]
Column 0  Weight=0.9964
Column 1  Weight=0.9964
Column 2  Weight=0.9964
Column 3  Weight=1.0108

[Table][0x1B7974CF,4]
Column 0  Weight=0.9964
Column 1  Weight=0.9964
Column 2  Weight=0.9964
Column 3  Weight=1.0108

[Docking][Data]
DockSpace               ID=0x8B93E3BD Window=0xA787BDB4 Pos=0,0 Size=1600,900 Split=X Selected=0x420EC169
  DockNode              ID=0x00000005 Parent=0x8B93E3BD SizeRef=1255,900 Split=X
    DockNode            ID=0x00000001 Parent=0x00000005 SizeRef=199,900 Split=Y Selected=0x420EC169
      DockNode          ID=0x00000002 Parent=0x00000001 SizeRef=797,189 Selected=0x222AE160
      DockNode          ID=0x00000004 Parent=0x00000001 SizeRef=797,709 Split=Y Selected=0x2E8B7B6D
        DockNode        ID=0x0000000D Parent=0x00000004 SizeRef=205,238 Split=Y Selected=0x26CE0345
          DockNode      ID=0x0000000F Parent=0x0000000D SizeRef=205,356 Split=Y Selected=0x420EC169
            DockNode    ID=0x00000011 Parent=0x0000000F SizeRef=205,466 Selected=0x420EC169
            DockNode    ID=0x00000012 Parent=0x0000000F SizeRef=205,241 Selected=0x169D4758
          DockNode      ID=0x00000010 Parent=0x0000000D SizeRef=205,351 Selected=0xDCC96A67
        DockNode        ID=0x0000000E Parent=0x00000004 SizeRef=205,245 Selected=0x835B01A0
    DockNode            ID=0x00000003 Parent=0x00000005 SizeRef=1054,900 Split=Y Selected=0xB652884E
      DockNode          ID=0x00000013 Parent=0x00000003 SizeRef=1017,592 CentralNode=1 Selected=0xB652884E
      DockNode          ID=0x00000014 Parent=0x00000003 SizeRef=1017,306 Selected=0x0FA953A6
  DockNode              ID=0x00000006 Parent=0x8B93E3BD SizeRef=343,900 Split=Y Selected=0x2EA171A3
    DockNode            ID=0x00000007 Parent=0x00000006 SizeRef=171,435 Split=X Selected=0x2EA171A3
      DockNode          ID=0x00000009 Parent=0x00000007 SizeRef=154,541 Selected=0xB6411A07
      DockNode          ID=0x0000000A Parent=0x00000007 SizeRef=188,541 Selected=0x2EA171A3
    DockNode            ID=0x00000008 Parent=0x00000006 SizeRef=171,463 Split=Y Selected=0x9A2414E3
      DockNode          ID=0x0000000B Parent=0x00000008 SizeRef=344,193 Split=Y Selected=0x9A2414E3
        DockNode        ID=0x00000017 Parent=0x0000000B SizeRef=153,191 Split=Y Selected=0x9A2414E3
          DockNode      ID=0x00000019 Parent=0x00000017 SizeRef=150,197 Split=Y Selected=0x9A2414E3
            DockNode    ID=0x0000001B Parent=0x00000019 SizeRef=169,212 Split=Y Selected=0x9A2414E3
              DockNode  ID=0x00000015 Parent=0x0000001B SizeRef=340,232 Selected=0x9A2414E3
              DockNode  ID=0x00000016 Parent=0x0000001B SizeRef=340,229 Selected=0xEA4F0E38
            DockNode    ID=0x0000001C Parent=0x00000019 SizeRef=169,249 Selected=0x9B6D4A67
          DockNode      ID=0x0000001A Parent=0x00000017 SizeRef=150,264 Selected=0x1751D27E
        DockNode        ID=0x00000018 Parent=0x0000000B SizeRef=153,270 Selected=0x94D3C4D0
      DockNode          ID=0x0000000C Parent=0x00000008 SizeRef=344,268 Selected=0x1F613405
)ini";
};

} // namespace cqde
