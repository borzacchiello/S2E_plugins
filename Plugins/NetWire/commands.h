//
// Created by luca on 17/01/19.
//

#ifndef SOURCE_COMMANDS_H
#define SOURCE_COMMANDS_H

#define CMD_00 0x4010ce
#define CMD_04 0x4010eb
#define CMD_05 0x4011e7
#define CMD_06 0x4012fc
#define CMD_08 0x40132a
#define CMD_09 0x40133c
#define CMD_10 0x40135d
#define CMD_11 0x401388
#define CMD_12 0x401395
#define CMD_13 0x401535
#define CMD_15 0x401545
#define CMD_17 0x401b52
#define CMD_19 0x401bea
#define CMD_20 0x401582
#define CMD_21 0x401710
#define CMD_22 0x401788
#define CMD_23 0x401938
#define CMD_24 0x4019b3
#define CMD_25 0x401a5a
#define CMD_26 0x401ae3
#define CMD_27 0x401b08
#define CMD_28 0x401b2d
#define CMD_29 0x401728
#define CMD_30 0x401788
#define CMD_31 0x401cae
#define CMD_32 0x401cda
#define CMD_33 0x401ccb
#define CMD_35 0x401ce7
#define CMD_37 0x401cf7
#define CMD_39 0x401d07
#define CMD_41 0x401d17
#define CMD_42 0x401d3c
#define CMD_43 0x401d4c
#define CMD_44 0x401d59
#define CMD_46 0x4017bc
#define CMD_47 0x4017d1
#define CMD_48 0x4017e6
#define CMD_49 0x40181e
#define CMD_50 0x401798
#define CMD_53 0x401856
#define CMD_55 0x4018f5
#define CMD_56 0x4018e8
#define CMD_57 0x4018d4
#define CMD_60 0x401e81
#define CMD_61 0x401e81
#define CMD_62 0x401eac
#define CMD_63 0x401eac
#define CMD_64 0x401ed7
#define CMD_65 0x401ed7
#define CMD_66 0x401f02
#define CMD_67 0x401f5b
#define CMD_69 0x401f68
#define CMD_71 0x401f95
#define CMD_72 0x401bf9
#define CMD_75 0x401c9f
#define CMD_DF 0x401f9a

typedef struct cmd_flags {
    bool cmd_00;
    bool cmd_04;
    bool cmd_05;
    bool cmd_06;
    bool cmd_08;
    bool cmd_09;
    bool cmd_10;
    bool cmd_11;
    bool cmd_12;
    bool cmd_13;
    bool cmd_15;
    bool cmd_17;
    bool cmd_19;
    bool cmd_20;
    bool cmd_21;
    bool cmd_22;
    bool cmd_23;
    bool cmd_24;
    bool cmd_25;
    bool cmd_26;
    bool cmd_27;
    bool cmd_28;
    bool cmd_29;
    bool cmd_31;
    bool cmd_32;
    bool cmd_33;
    bool cmd_35;
    bool cmd_37;
    bool cmd_39;
    bool cmd_41;
    bool cmd_42;
    bool cmd_43;
    bool cmd_44;
    bool cmd_46;
    bool cmd_47;
    bool cmd_48;
    bool cmd_49;
    bool cmd_50;
    bool cmd_53;
    bool cmd_55;
    bool cmd_56;
    bool cmd_57;
    bool cmd_60;
    bool cmd_62;
    bool cmd_64;
    bool cmd_66;
    bool cmd_67;
    bool cmd_69;
    bool cmd_71;
    bool cmd_72;
    bool cmd_75;
} cmd_flags;

#endif //SOURCE_COMMANDS_H
