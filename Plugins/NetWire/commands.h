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

bool noMoreCommands(cmd_flags* flags) {
    return flags->cmd_05 && flags->cmd_06 && flags->cmd_08 && flags->cmd_09 && flags->cmd_10 && flags->cmd_11 &&
    flags->cmd_12 && flags->cmd_13 && flags->cmd_15 && flags->cmd_17 && flags->cmd_19 && flags->cmd_20 &&
    flags->cmd_21 && flags->cmd_22 && flags->cmd_23 && flags->cmd_24 && flags->cmd_25 && flags->cmd_26 &&
    flags->cmd_27 && flags->cmd_28 && flags->cmd_29 && flags->cmd_31 && flags->cmd_32 && flags->cmd_33 &&
    flags->cmd_35 && flags->cmd_37 && flags->cmd_39 && flags->cmd_41 && flags->cmd_42 && flags->cmd_43 &&
    flags->cmd_44 && flags->cmd_46 && flags->cmd_47 && flags->cmd_48 && flags->cmd_49 && flags->cmd_50 &&
    flags->cmd_53 && flags->cmd_55 && flags->cmd_56 && flags->cmd_57 && flags->cmd_60 && flags->cmd_62 &&
    flags->cmd_64 && flags->cmd_66 && flags->cmd_67 && flags->cmd_69 && flags->cmd_71 && flags->cmd_72 &&
    flags->cmd_75;
}

#endif //SOURCE_COMMANDS_H
