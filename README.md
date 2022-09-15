# SknGarageDoor
Homie Node developed using Automaton State Machines and VL53L1x


## OpenHab3 Console Log

    2022-09-14 20:57:09.780 [INFO ] [openhab.event.ItemCommandEvent      ] - Item 'GarageDoor_Position' received command UP
    2022-09-14 20:57:09.788 [INFO ] [penhab.event.ItemStatePredictedEvent] - Item 'GarageDoor_Position' predicted to become UP
    2022-09-14 20:57:13.870 [INFO ] [openhab.event.ItemCommandEvent      ] - Item 'GarageDoor_Position' received command DOWN
    2022-09-14 20:57:13.878 [INFO ] [penhab.event.ItemStatePredictedEvent] - Item 'GarageDoor_Position' predicted to become DOWN
    2022-09-14 20:57:13.886 [INFO ] [openhab.event.ItemStateChangedEvent ] - Item 'GarageDoor_Position' changed from 0 to 100
    2022-09-14 20:57:17.836 [INFO ] [openhab.event.ItemCommandEvent      ] - Item 'GarageDoor_Position' received command STOP

## ESP8266 Console Log

    21:01:59.886 > 74624596 Switch DOOR@3FFEEDA8 from STOPPED to MOVING_UP on EVT_UP (221812990 cycles in 9510428 ms)
    21:02:00.315 > 〽 range: 1725 mm avgerage: 1361 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:00.323 > 〽 range: 1725 mm avgerage: 1438 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:00.331 > 〽 range: 1725 mm avgerage: 1515 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:00.340 > SknAtmDoor::setDoorPosition(115:-1) Position:63, Moving:MOVING_DOWN, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:39, B:63
    21:02:01.143 > 〽 range: 1688 mm avgerage: 1578 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:01.151 > 〽 range: 1688 mm avgerage: 1642 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:01.159 > 〽 range: 1688 mm avgerage: 1706 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:01.167 > SknAtmDoor::setDoorPosition(116:-1) Position:62, Moving:MOVING_DOWN, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:40, B:62
    21:02:02.062 > 〽 range: 1672 mm avgerage: 1697 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:02.070 > 〽 range: 1672 mm avgerage: 1688 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:02.078 > 〽 range: 1672 mm avgerage: 1680 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:02.086 > SknAtmDoor::setDoorPosition(117:-1) Position:61, Moving:MOVING_DOWN, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:41, B:61
    21:02:02.983 > 〽 range: 1667 mm avgerage: 1676 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:02.991 > 〽 range: 1667 mm avgerage: 1673 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:02.999 > 〽 range: 1667 mm avgerage: 1669 mm,     status: range valid     raw: 0  signal: 3.1 MCPS        ambient: 0.2 MCPS
    21:02:03.007 > SknAtmDoor::setDoorPosition(118:-1) Position:61, Moving:MOVING_DOWN, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:43, B:61
    21:02:03.902 > 〽 range: 1587 mm avgerage: 1655 mm,     status: range valid     raw: 0  signal: 3.3 MCPS        ambient: 0.2 MCPS
    21:02:03.910 > 〽 range: 1587 mm avgerage: 1641 mm,     status: range valid     raw: 0  signal: 3.3 MCPS        ambient: 0.2 MCPS
    21:02:03.919 > 〽 range: 1587 mm avgerage: 1627 mm,     status: range valid     raw: 0  signal: 3.3 MCPS        ambient: 0.2 MCPS
    21:02:03.927 > SknAtmDoor::setDoorPosition(119:-1) Position:57, Moving:MOVING_DOWN, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:44, B:57
    21:02:04.822 > 〽 range: 1557 mm avgerage: 1608 mm,     status: range valid     raw: 0  signal: 3.5 MCPS        ambient: 0.2 MCPS
    21:02:04.831 > 〽 range: 1557 mm avgerage: 1590 mm,     status: range valid     raw: 0  signal: 3.5 MCPS        ambient: 0.2 MCPS
    21:02:04.839 > 〽 range: 1557 mm avgerage: 1572 mm,     status: range valid     raw: 0  signal: 3.5 MCPS        ambient: 0.2 MCPS
    21:02:04.848 > SknAtmDoor::setDoorPosition(120:-1) Position:56, Moving:MOVING_UP, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:63, B:56
    21:02:05.742 > 〽 range: 1506 mm avgerage: 1558 mm,     status: range valid     raw: 0  signal: 3.8 MCPS        ambient: 0.2 MCPS
    21:02:05.750 > 〽 range: 1506 mm avgerage: 1545 mm,     status: range valid     raw: 0  signal: 3.8 MCPS        ambient: 0.2 MCPS
    21:02:05.759 > 〽 range: 1506 mm avgerage: 1531 mm,     status: range valid     raw: 0  signal: 3.8 MCPS        ambient: 0.2 MCPS
    21:02:05.767 > SknAtmDoor::setDoorPosition(121:-1) Position:54, Moving:MOVING_UP, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:62, B:54
    21:02:06.663 > 〽 range: 1478 mm avgerage: 1518 mm,     status: range valid     raw: 0  signal: 4.0 MCPS        ambient: 0.2 MCPS
    21:02:06.671 > 〽 range: 1478 mm avgerage: 1505 mm,     status: range valid     raw: 0  signal: 4.0 MCPS        ambient: 0.2 MCPS
    21:02:06.680 > 〽 range: 1478 mm avgerage: 1492 mm,     status: range valid     raw: 0  signal: 4.0 MCPS        ambient: 0.2 MCPS
    ...
    21:02:17.731 > SknAtmDoor::setDoorPosition(134:-1) Position:35, Moving:MOVING_UP, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:42, B:35
    21:02:18.627 > 〽 range: 1097 mm avgerage: 1105 mm,     status: range valid     raw: 0  signal: 16.1 MCPS       ambient: 0.1 MCPS
    21:02:18.635 > 〽 range: 1097 mm avgerage: 1103 mm,     status: range valid     raw: 0  signal: 16.1 MCPS       ambient: 0.1 MCPS
    21:02:18.644 > 〽 range: 1097 mm avgerage: 1100 mm,     status: range valid     raw: 0  signal: 16.1 MCPS       ambient: 0.1 MCPS
    21:02:18.652 > SknAtmDoor::setDoorPosition(135:-1) Position:35, Moving:MOVING_UP, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:42, B:35
    21:02:19.547 > 〽 range: 1088 mm avgerage: 1097 mm,     status: range valid     raw: 0  signal: 15.8 MCPS       ambient: 0.1 MCPS
    21:02:19.555 > 〽 range: 1088 mm avgerage: 1095 mm,     status: range valid     raw: 0  signal: 15.8 MCPS       ambient: 0.1 MCPS
    21:02:19.563 > 〽 range: 1088 mm avgerage: 1092 mm,     status: range valid     raw: 0  signal: 15.8 MCPS       ambient: 0.1 MCPS
    21:02:19.572 > SknAtmDoor::setDoorPosition(136:-1) Position:34, Moving:MOVING_UP, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:39, B:34
    21:02:20.468 > 〽 range: 1052 mm avgerage: 1085 mm,     status: range valid     raw: 0  signal: 12.3 MCPS       ambient: 0.1 MCPS
    21:02:20.476 > 〽 range: 1052 mm avgerage: 1077 mm,     status: range valid     raw: 0  signal: 12.3 MCPS       ambient: 0.1 MCPS
    21:02:20.484 > 〽 range: 1052 mm avgerage: 1070 mm,     status: range valid     raw: 0  signal: 12.3 MCPS       ambient: 0.1 MCPS
    21:02:20.493 > SknAtmDoor::setDoorPosition(137:-1) Position:33, Moving:MOVING_UP, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:37, B:33
    21:02:21.388 > 〽 range: 1034 mm avgerage: 1061 mm,     status: range valid     raw: 0  signal: 11.3 MCPS       ambient: 0.1 MCPS
    21:02:21.396 > 〽 range: 1034 mm avgerage: 1052 mm,     status: range valid     raw: 0  signal: 11.3 MCPS       ambient: 0.1 MCPS
    21:02:21.404 > 〽 range: 1034 mm avgerage: 1043 mm,     status: range valid     raw: 0  signal: 11.3 MCPS       ambient: 0.1 MCPS
    21:02:21.413 > SknAtmDoor::setDoorPosition(138:-1) Position:32, Moving:MOVING_UP, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:36, B:32
    21:02:22.309 > 〽 range: 1031 mm avgerage: 1039 mm,     status: range valid     raw: 0  signal: 10.9 MCPS       ambient: 0.1 MCPS
    21:02:22.317 > 〽 range: 1031 mm avgerage: 1036 mm,     status: range valid     raw: 0  signal: 10.9 MCPS       ambient: 0.1 MCPS
    21:02:22.325 > 〽 range: 1031 mm avgerage: 1032 mm,     status: range valid     raw: 0  signal: 10.9 MCPS       ambient: 0.1 MCPS
    21:02:22.334 > SknAtmDoor::setDoorPosition(139:-1) Position:32, Moving:MOVING_UP, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:35, B:32
    21:02:23.228 > 〽 range: 984 mm avgerage: 1024 mm,      status: range valid     raw: 0  signal: 9.3 MCPS        ambient: 0.1 MCPS
    21:02:23.237 > 〽 range: 984 mm avgerage: 1015 mm,      status: range valid     raw: 0  signal: 9.3 MCPS        ambient: 0.1 MCPS
    21:02:23.245 > 〽 range: 984 mm avgerage: 1007 mm,      status: range valid     raw: 0  signal: 9.3 MCPS        ambient: 0.1 MCPS
    21:02:23.253 > SknAtmDoor::setDoorPosition(140:-1) Position:30, Moving:MOVING_UP, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:35, B:30
    21:02:24.149 > 〽 range: 349 mm avgerage: 893 mm,       status: range valid     raw: 0  signal: 6.8 MCPS        ambient: 0.1 MCPS
    21:02:24.158 > 〽 range: 349 mm avgerage: 780 mm,       status: range valid     raw: 0  signal: 6.8 MCPS        ambient: 0.1 MCPS
    21:02:24.166 > 〽 range: 349 mm avgerage: 666 mm,       status: range valid     raw: 0  signal: 6.8 MCPS        ambient: 0.1 MCPS
    21:02:24.174 > SknAtmDoor::setDoorPosition(141:-1) Position:1, Moving:MOVING_UP, sReq:MOVING_UP, sCur:MOVING_UP, sNext:ELSE, chgDir:False, A:34, B:1
    21:02:24.185 > 74648892 Switch DOOR@3FFEEDA8 from MOVING_UP to UP on EVT_POS_REACHED (524780 cycles in 24294 ms)
