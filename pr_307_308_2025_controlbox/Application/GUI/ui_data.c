/*********************
 *      INCLUDES
 *********************/
#include "ui_config.h"
#include "ui_assets.h"
#include "ui_data.h"
#include "page_manager.h"
#include "app_data_center.h"
#include "app_eth.h"
#include "lfm_core.h"
#include "app_ble_port.h"
#include "dev_crmx.h"
#include "ui_hal.h"
#include "app_artnet.h"
#include "user_sacn.h"
/*********************
 *      TPEDEFS
 *********************/
/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool preset_task_light_mode_valid(enum light_mode mode);
/*********************
 *  STATIC VATIABLES
 *********************/
const gel_dsc_t LEE_ColorCorrection[] = {
    { "Double\nCTB", 0x80aff1, 200, 0, 1, 6},
    { "Full\nCTB", 0xb7d9f9, 201, 2, 3, 6},
    { "3/4\nCTB", 0xc2e0f9, 281, 4, 5, 6},
    { "1/2\nCTB", 0xceecfe, 202, 6, 7, 6},
    { "1/4\nCTB", 0xe7fcff, 203, 8, 9, 6},
    { "1/8\nCTB", 0xf3ffff, 218, 10, 11, 6},
    { "Double\nCTO", 0xfc8e44, 287, 12, 13, 6},
    { "Full\nCTO", 0xf7b675, 204, 14, 15, 6},
    { "3/4\nCTO", 0xf9c282, 285, 16, 17, 6},
    { "1/2\nCTO", 0xfad1a1, 205, 18, 19, 6},
    { "1/4\nCTO", 0xfbe5cc, 206, 20, 21, 6},
    { "1/8\nCTO", 0xfff0e3, 223, 22, 23, 6},
    { "1\n1/2\nCTB", 0xa0cff2, 283, 24, 25, 6},
    { "1\n1/2\nCTO", 0xfda452, 286, 26, 27, 6},
    { "Full\nCTS", 0xf7b371, 441, 28, 29, 6},
    { "1/2\nCTS", 0xfad59d, 442, 30, 31, 6},
    { "1/4\nCTS", 0xfbebd2, 443, 32, 33, 6},
    { "1/8\nCTS", 0xf8f0e3, 444, 34, 35, 6},
    { "Full\nCTO\n+ .3 ND", 0xeba559, 207, 36, 37, 6},
    { "Full\nCTO\n+ .6 ND", 0xde943f, 208, 38, 39, 6},
    { "L.C.T.Yellow(Y1)", 0xffface, 212, 40, 41, 6},
    { "White\nFlame\nGreen", 0xe0fdd3, 213, 42, 43, 6},
    { "LEE\nFluorescent\nGreen", 0x8cd6a0, 219, 44, 45, 6},
    { "Super\nCorrection\nL.C.T.Yellow", 0xd1b07a, 230, 46, 47, 6},
    { "Super\nCorrection\nW.F.Green", 0xe7b171, 232, 48, 49, 6},
    { "H.M.I.(to\nTungsten)", 0xf8bb83, 236, 50, 51, 6},
    { "C.I.D.(to\nTungsten)", 0xf9a180, 237, 52, 53, 6},
    { "C.S.I.(to\nTungsten)", 0xf1aa99, 238, 54, 55, 6},
    { "LEE\nFluorescent\n5700\nKelvin", 0x9bd6b6, 241, 56, 57, 6},
    { "LEE\nFluorescent\n4300\nKelvin", 0xade2ab, 242, 58, 59, 6},
    { "LEE\nFluorescent\n3600\nKelvin", 0xc3f69f, 243, 60, 61, 6},
    { "LEE\nPlus\nGreen", 0xe0fcb0, 244, 62, 63, 6},
    { "1/2\nPlus\nGreen", 0xedfdc8, 245, 64, 65, 6},
    { "1/4\nPlus\nGreen", 0xf3ffe0, 246, 66, 67, 6},
    { "1/8\nPlus\nGreen", 0xf0ffef, 278, 68, 69, 6},
    { "LEE\nMinus\nGreen", 0xf8b3ce, 247, 70, 71, 6},
    { "1/2\nMinus\nGreen", 0xfedadd, 248, 72, 73, 6},
    { "1/4\nMinus\nGreen", 0xfee6ec, 249, 74, 75, 6},
    { "1/8\nMinus\nGreen", 0xfeeff5, 279, 76, 77, 6},
};

const gel_dsc_t LEE_ColorFilters[] = {
    { "Rose\nPink", 0xfc5ad4, 2, 0, 1, 6},
    { "Lavender\nTint", 0xf8ebf9, 3, 2, 3, 6},
    { "Medium\nBastard\nAmber", 0xfebba5, 4, 4, 5, 6},
    { "Pale\nYellow", 0xf9fbc8, 7, 6, 7, 6},
    { "Dark\nSalmon", 0xf9876e, 8, 8, 9, 6},
    { "Pale\nAmber\nGold", 0xfacea2, 9, 10, 11, 6},
    { "Medium\nYellow", 0xffff28, 10, 12, 13, 6},
    { "Straw\nTint", 0xfbd08a, 13, 14, 15, 6},
    { "Surprise\nPeach", 0xde7752, 17, 16, 17, 6},
    { "Fire", 0xfc2d13, 19, 18, 19, 6},
    { "Medium\nAmber", 0xfeb145, 20, 20, 21, 6},
    { "Gold\nAmber", 0xfd7827, 21, 22, 23, 6},
    { "Dark\nAmber", 0xfc5216, 22, 24, 25, 6},
    { "Scarlet", 0xfc4052, 24, 26, 27, 6},
    { "Sunset\nRed", 0xfc5637, 25, 28, 29, 6},
    { "Bright\nRed", 0xdc0d2f, 26, 30, 31, 6},
    { "Light\nPink", 0xfeb9c8, 35, 32, 33, 6},
    { "Medium\nPink", 0xfd8bab, 36, 34, 35, 6},
    { "Dark\nMagenta", 0xd00b49, 46, 36, 37, 6},
    { "Rose\nPurple", 0xdd539f, 48, 38, 39, 6},
    { "Light\nLavender", 0xde94f8, 52, 40, 41, 6},
    { "Paler\nLavender", 0xdfdff9, 53, 42, 43, 6},
    { "Lavender", 0xa351ec, 58, 44, 45, 6},
    { "Mist\nBlue", 0xd4f3fe, 61, 46, 47, 6},
    { "Pale\nBlue", 0xc9f3fe, 63, 48, 49, 6},
    { "Sky\nBlue", 0x3ba4ec, 68, 50, 51, 6},
    { "Evening\nBlue", 0x538bec, 75, 52, 53, 6},
    { "Just\nBlue", 0x3078c7, 79, 54, 55, 6},
    { "Deeper\nBlue", 0x0c58aa, 85, 56, 57, 6},
    { "Lime\nGreen", 0xd5ff52, 88, 58, 59, 6},
    { "Moss\nGreen", 0x50d849, 89, 60, 61, 6},
    { "Dark\nYellow\nGreen", 0x18b519, 90, 62, 63, 6},
    { "Spring\nYellow", 0xf4ff28, 100, 64, 65, 6},
    { "Yellow", 0xfef727, 101, 66, 67, 6},
    { "Light\nAmber", 0xfed64e, 102, 68, 69, 6},
    { "Straw", 0xfbe6c1, 103, 70, 71, 6},
    { "Deep\nAmber", 0xfed723, 104, 72, 73, 6},
    { "Primary\nRed", 0xe80e26, 106, 74, 75, 6},
    { "Light\nRose", 0xfd91a0, 107, 76, 77, 6},
    { "English\nRose", 0xf9af99, 108, 78, 79, 6},
    { "Light\nSalmon", 0xfda19f, 109, 80, 81, 6},
    { "Middle\nRose", 0xfda1bc, 110, 82, 83, 6},
    { "Dark\nPink", 0xfc74b0, 111, 84, 85, 6},
    { "Magenta", 0xfb0e52, 113, 86, 87, 6},
    { "Peacock\nBlue", 0x1deabc, 115, 88, 89, 6},
    { "Steel\nBlue", 0xa7fbf3, 117, 90, 91, 6},
    { "Light\nBlue", 0x1ddbe6, 118, 92, 93, 6},
    { "Deep\nBlue", 0x0b49b0, 120, 94, 95, 6},
    { "LEE\nGreen", 0xa8ff51, 121, 96, 97, 6},
    { "Fern\nGreen", 0x6bff5b, 122, 98, 99, 6},
    { "Dark\nGreen", 0x1dd965, 124, 100, 101, 6},
    { "Smokey\nPink", 0xd74a60, 127, 102, 103, 6},
    { "Bright\nPink", 0xfb30a5, 128, 104, 105, 6},
    { "Marine\nBlue", 0x58fdc8, 131, 106, 107, 6},
    { "Golden\nAmber", 0xf69660, 134, 108, 109, 6},
    { "Deep\nGolden\nAmber", 0xfb4915, 135, 110, 111, 6},
    { "Pale\nLavender", 0xeaade0, 136, 112, 113, 6},
    { "Special\nLavender", 0xbba2df, 137, 114, 115, 6},
    { "Pale\nGreen", 0xd5ff8f, 138, 116, 117, 6},
    { "Summer\nBlue", 0x79efe5, 140, 118, 119, 6},
    { "Pale\nViolet", 0x9996ec, 142, 120, 121, 6},
    { "Pale\nNave\nBlue", 0x54b1c8, 143, 122, 123, 6},
    { "No Color\nBlue", 0x4edbe0, 144, 124, 125, 6},
    { "Apricot", 0xf9aa7a, 147, 126, 127, 6},
    { "Bright\nRose", 0xfb346a, 148, 128, 129, 6},
    { "Gold\nTint", 0xfec1b6, 151, 130, 131, 6},
    { "Pale\nGold", 0xfec7b4, 152, 132, 133, 6},
    { "Pale\nSalmon", 0xfdbbbc, 153, 134, 135, 6},
    { "Pale\nRose", 0xfecbc4, 154, 136, 137, 6},
    { "Chocolate", 0xd9a579, 156, 138, 139, 6},
    { "Pink", 0xfc7b93, 157, 140, 141, 6},
    { "No\nColor\nStraw", 0xfff9e6, 159, 142, 143, 6},
    { "Slate\nBlue", 0x6cc7f2, 161, 144, 145, 6},
    { "Bastard\nAmber", 0xfbd5cf, 162, 146, 147, 6},
    { "Flame\nRed", 0xfb1712, 164, 148, 149, 6},
    { "Daylight\nBlue", 0x4dbce6, 165, 150, 151, 6},
    { "Lilac\nTint", 0xf8d2ec, 169, 152, 153, 6},
    { "Deep\nLavender", 0xde96d4, 170, 154, 155, 6},
    { "Dark\nSteel\nBlue", 0xa0d9f8, 174, 156, 157, 6},
    { "Loving\nLavender", 0xfd978f, 176, 158, 159, 6},
    { "Dark\nLavender", 0x8d48df, 180, 160, 161, 6},
    { "Light\nRed", 0xef0f11, 182, 162, 163, 6},
    { "Flesh\nPink", 0xfc74a5, 192, 164, 165, 6},
    { "Surprise\nPink", 0xaf73ec, 194, 166, 167, 6},
    { "Zenith\nBlue", 0x0a57c8, 195, 168, 169, 6},
    { "True\nBlue", 0x6ac8df, 196, 170, 171, 6},
    { "Alice\nBlue", 0x7098df, 197, 172, 173, 6},
    { "Palace\nBlue", 0x5162bb, 198, 174, 175, 6},
    { "Regal\nBlue", 0x5149df, 199, 176, 177, 6},
};

const gel_dsc_t LEE_600Series[] = {
    { "Arctic\nWhite", 0x86989a, 600, 0, 1, 6},
    { "Silver", 0x7e8f8f, 601, 2, 3, 6},
    { "Platinum", 0x94949a, 602, 4, 5, 6},
    { "Moonlight\nWhite", 0xbcc2c2, 603, 6, 7, 6},
    { "Full\nCT\n85", 0xfeb483, 604, 8, 9, 6},
    { "Industry\nSodium", 0xbb9950, 650, 10, 11, 6},
    { "HI\nSodium", 0xfd884d, 651, 12, 13, 6},
    { "Urban\nSodium", 0xfc6c37, 652, 14, 15, 6},
    { "LO\nSodium", 0x83430e, 653, 16, 17, 6},
};
const gel_dsc_t LEE_CosmeticFilters[] = {
    { "Cosmetic\nPeach", 0xecd9ce, 184, 0, 1, 6},
    { "Cosmetic\nSilver\nRose", 0xf5d3d5, 186, 2, 3, 6},
    { "Cosmetic\nRouge", 0xf1ddce, 187, 4, 5, 6},
    { "Cosmetic\nHighlight", 0xfee6cd, 188, 6, 7, 6},
    { "Cosmetic\nSilver\nMoss", 0xeae9cf, 189, 8, 9, 6},
    { "Cosmetic\nAqua\nBlue", 0xe3eaea, 191, 10, 11, 6},
    { "Lily\nFrost", 0xea93f8, 705, 12, 13, 6},
    { "Shanklin\nFrost", 0xb7d9f9, 717, 14, 15, 6},
    { "Half\nShanklin\nFrost", 0xcae9f8, 718, 16, 17, 6},
    { "Durham\nDaylight\nFrost", 0xb7d9f9, 720, 18, 19, 6},
    { "Hampshire\nRose", 0xfac7c2, 749, 20, 21, 6},
    { "Durham\nFrost", 0xf9edfd, 750, 22, 23, 6},
    { "Soft\nAmber\nKey\n1", 0xfac8b0, 774, 24, 25, 6},
    { "Soft\nAmber\nKey\n2", 0xfab994, 775, 26, 27, 6},
    { "Moroccan\nFrost", 0xf1aa99, 791, 28, 29, 6},
    { "Blue\nDiffusion", 0xf3ffff, 217, 30, 31, 6},
    { "Blue\nFrost", 0xf3ffff, 221, 32, 33, 6},
    { "Daylight\nBlue\nFrost", 0x848eb0, 224, 34, 35, 6},
};
const gel_dsc_t LEE_700Series[] = {
    { "Perfect\nLavender", 0x6800c1, 700, 0, 1, 6},
    { "Provence", 0xa33cd4, 701, 2, 3, 6},
    { "Special\nPale\nLavender", 0xdfc6ec, 702, 4, 5, 6},
    { "Cold\nLavender", 0xc568d3, 703, 6, 7, 6},
    { "Lily", 0xea93f8, 704, 8, 9, 6},
    { "King\nFals\nLavender", 0x5a2cbc, 706, 10, 11, 6},
    { "Cool\nLavender", 0xd4e6f8, 708, 12, 13, 6},
    { "Electric\nLilac", 0xbbb9f9, 709, 14, 15, 6},
    { "Spir\nSpecial\nBlue", 0x798bec, 710, 16, 17, 6},
    { "Cold\nBlue", 0x9bafd4, 711, 18, 19, 6},
    { "Bedford\nBlue", 0x8faeec, 712, 20, 21, 6},
    { "Elysian\nBlue", 0x497dc7, 714, 22, 23, 6},
    { "Cabana\nBlue", 0x2e56d3, 715, 24, 25, 6},
    { "Mikkel\nBlue", 0x094cc7, 716, 26, 27, 6},
    { "Colour\nWash\nBlue", 0x8ab5ec, 719, 28, 29, 6},
    { "Berry\nBlue", 0x0b6bdf, 721, 30, 31, 6},
    { "Virgin\nBlue", 0x406bd3, 723, 32, 33, 6},
    { "Ocean\nBlue", 0x5bdbe6, 724, 34, 35, 6},
    { "Old\nSteel\nBlue", 0xb2f0ef, 725, 36, 37, 6},
    { "Steel\nGreen", 0xbde8c8, 728, 38, 39, 6},
    { "Liberty\nGreen", 0xd5ffe0, 730, 40, 41, 6},
    { "Dirty\nIce", 0xdbfacd, 731, 42, 43, 6},
    { "Damp\nSquib", 0xe7f6c4, 733, 44, 45, 6},
    { "JAS\nGreen", 0x9dff25, 738, 46, 47, 6},
    { "Am\nBrown", 0xd88940, 742, 48, 49, 6},
    { "Dirty\nWhite", 0xf5bc70, 744, 50, 51, 6},
    { "Brown", 0x5a2d09, 746, 52, 53, 6},
    { "Easy\nWhite", 0xf1aa8e, 747, 54, 55, 6},
    { "Seedy\nPink", 0xe04e72, 748, 56, 57, 6},
    { "Wheat", 0xfbedc8, 763, 58, 59, 6},
    { "Sun\nColour\nStraw", 0xfbe1a5, 764, 60, 61, 6},
    { "LEE\nYellow", 0xfee07f, 765, 62, 63, 6},
    { "Cardbox\nAmber", 0xf9b8a3, 773, 64, 65, 6},
    { "Nectarine", 0xf9b08a, 776, 66, 67, 6},
    { "Millenium\nGold", 0xfc5f17, 778, 68, 69, 6},
    { "Bastard\nPink", 0xf98372, 779, 70, 71, 6},
    { "Terry\nRed", 0xfc3714, 781, 72, 73, 6},
    { "Blood\nRed", 0x982a26, 789, 74, 75, 6},
    { "Moroccan\nPink", 0xfda395, 790, 76, 77, 6},
    { "Pretty\n’n’Pink", 0xfd9bd4, 794, 78, 79, 6},
    { "Magical\nMagenta", 0xf521bc, 795, 80, 81, 6},
};
const gel_dsc_t ROSCO_ColorCorrection[] = {
    { "Full\nCTB", 0x7d88fb, 3202, 0, 1, 6},
    { "3/4\nCTB", 0xa6adfc, 3203, 2, 3, 6},
    { "1/2\nCTB", 0xbec4fd, 3204, 4, 5, 6},
    { "1/3\nCTB", 0xccd2fd, 3206, 6, 7, 6},
    { "1/4\nCTB", 0xe2e7fe, 3208, 8, 9, 6},
    { "1/8\nCTB", 0xeaedfe, 3216, 10, 11, 6},
    { "Double\nCTB", 0x2c35fa, 3220, 12, 13, 6},
    { "Full\nCTO", 0xf37919, 3407, 14, 15, 6},
    { "3/4\nCTO", 0xf39a3f, 3411, 16, 17, 6},
    { "1/2\nCTO", 0xf9b574, 3408, 18, 19, 6},
    { "1/4\nCTO", 0xffdca7, 3409, 20, 21, 6},
    { "1/8\nCTO", 0xfeedd1, 3410, 22, 23, 6},
    { "Double\nCTO", 0xfc5516, 3420, 24, 25, 6},
    { "Full\nCTS", 0xf3a13f, 3441, 26, 27, 6},
    { "1/2\nCTS", 0xfec998, 3442, 28, 29, 6},
    { "1/4\nCTS", 0xfee2c6, 3443, 30, 31, 6},
    { "1/8\nCTS", 0xffeee0, 3444, 32, 33, 6},
    { "Full\nPlusgreen", 0xd6ff80, 3304, 34, 35, 6},
    { "1/2\nPlusgreen", 0xedffac, 3315, 36, 37, 6},
    { "1/4\nPlusgreen", 0xefffbf, 3316, 38, 39, 6},
    { "1/8\nPlusgreen", 0xf8ffdc, 3317, 40, 41, 6},
    { "Full\nMinusgreen", 0xf39ec0, 3308, 42, 43, 6},
    { "3/4\nMinusgreen", 0xfdaed1, 3309, 44, 45, 6},
    { "1/2\nMinusgreen", 0xf6b6cf, 3313, 46, 47, 6},
    { "1/4\nMinusgreen", 0xf9ccdc, 3314, 48, 49, 6},
    { "1/8\nMinusgreen", 0xfce5f0, 3318, 50, 51, 6},
    { "Fluorofilter", 0xf06559, 3310, 52, 53, 6},
    { "Industrial\nVapor", 0xd0cd61, 3150, 54, 55, 6},
    { "Urban\nVapor", 0xef6b22, 3152, 56, 57, 6},
    { "Tough\nY1", 0xfbf9d3, 3107, 58, 59, 6},
    { "Tough\nMT\n54", 0xffd9a9, 3134, 60, 61, 6},
    { "Tough\nMTY", 0xfc7f35, 3106, 62, 63, 6},
    { "Tough\nMT2", 0xfeb76b, 3102, 64, 65, 6},
};
const gel_dsc_t ROSCO_CalColor[] = {
    { "15\nBlue", 0xbebafd, 4215, 0, 1, 6},
    { "30\nBlue", 0x9080fb, 4230, 2, 3, 6},
    { "60\nBlue", 0x6140fa, 4260, 4, 5, 6},
    { "90\nBlue", 0x4300ed, 4290, 6, 7, 6},
    { "7\nCyan", 0xecfaff, 4307, 8, 9, 6},
    { "15\nCyan", 0xe1f3f8, 4315, 10, 11, 6},
    { "30\nCyan", 0xacdde7, 4330, 12, 13, 6},
    { "60\nCyan", 0x5dc0d1, 4360, 14, 15, 6},
    { "90\nCyan", 0x36acbe, 4390, 16, 17, 6},
    { "15\nGreen", 0xc4ff8c, 4415, 18, 19, 6},
    { "30\nGreen", 0x89f748, 4430, 20, 21, 6},
    { "60\nGreen", 0x3ada1e, 4460, 22, 23, 6},
    { "90\nGreen", 0x26af18, 4490, 24, 25, 6},
    { "15\nYellow", 0xfffdc8, 4515, 26, 27, 6},
    { "30\nYellow", 0xfffbb6, 4530, 28, 29, 6},
    { "60\nYellow", 0xfff772, 4560, 30, 31, 6},
    { "90\nYellow", 0xfff427, 4590, 32, 33, 6},
    { "15\nRed", 0xf0aca0, 4615, 34, 35, 6},
    { "30\nRed", 0xf19b8e, 4630, 36, 37, 6},
    { "60\nRed", 0xfc6148, 4660, 38, 39, 6},
    { "90\nRed", 0xfb0f21, 4690, 40, 41, 6},
    { "15\nMagenta", 0xf4b1cf, 4715, 42, 43, 6},
    { "30\nMagenta", 0xf08eba, 4730, 44, 45, 6},
    { "60\nMagenta", 0xeb599c, 4760, 46, 47, 6},
    { "90\nMagenta", 0xe40a79, 4790, 48, 49, 6},
    { "15\nPink", 0xf4a9bb, 4815, 50, 51, 6},
    { "30\nPink", 0xf0849b, 4830, 52, 53, 6},
    { "60\nPink", 0xec5777, 4860, 54, 55, 6},
    { "90\nPink", 0xe82e58, 4890, 56, 57, 6},
    { "15\nLavender", 0xd7a6fa, 4915, 58, 59, 6},
    { "30\nLavender", 0xc77dfc, 4930, 60, 61, 6},
    { "60\nLavender", 0xa035f1, 4960, 62, 63, 6},
    { "90\nLavender", 0x8500d8, 4990, 64, 65, 6},
};

const gel_dsc_t ROSCO_StoraroSelection[] = {
    { "VS\nRed", 0xd60d0f, 2001, 0, 1, 6},
    { "VS\nOrange", 0xfc5516, 2002, 2, 3, 6},
    { "VS\nYellow", 0xfec221, 2003, 4, 5, 6},
    { "VS\nGreen", 0x1d8312, 2004, 6, 7, 6},
    { "VS\nCyan", 0x118397, 2005, 8, 9, 6},
    { "VS\nAzure", 0x0028fa, 2006, 10, 11, 6},
    { "VS\nBlue", 0x2c35fa, 2007, 12, 13, 6},
    { "VS\nIndigo", 0x2f00ca, 2008, 14, 15, 6},
    { "VS\nViolet", 0x4f007f, 2009, 16, 17, 6},
    { "VS\nMagenta", 0xe40a79, 2010, 18, 19, 6},
};

const gel_dsc_t ROSCO_Cinelux[] = {
    { "Bastard\nAmber", 0xfec79c, 2, 0, 1, 6},
    { "Pale\nBastard\nAmber", 0xfddfc4, 302, 2, 3, 6},
    { "No\nColor\nStraw", 0xfbf9d3, 6, 4, 5, 6},
    { "Pale\nGold", 0xffd9a9, 8, 6, 7, 6},
    { "Daffodil", 0xffef40, 310, 8, 9, 6},
    { "Straw", 0xfff73d, 12, 10, 11, 6},
    { "Light\nAmber", 0xfeb76b, 16, 12, 13, 6},
    { "Gallo\nGold", 0xf39a3f, 316, 14, 15, 6},
    { "Light\nFlame", 0xfc7f3c, 17, 16, 17, 6},
    { "Flame", 0xfc7f35, 18, 18, 19, 6},
    { "Mayan\nSun", 0xfc5820, 318, 20, 21, 6},
    { "Golden\nAmber", 0xfc4f16, 21, 22, 23, 6},
    { "Soft\nGolden\nAmber", 0xfc812f, 321, 24, 25, 6},
    { "Orange", 0xfc4215, 23, 26, 27, 6},
    { "Henna\nSky", 0xd91310, 325, 28, 29, 6},
    { "Light\nRed", 0xcb0d20, 26, 30, 31, 6},
    { "No\nColor\nPink", 0xfdb2c5, 33, 32, 33, 6},
    { "Blush\nPink", 0xf6b6cf, 333, 34, 35, 6},
    { "Flesh\nPink", 0xfd767b, 34, 36, 37, 6},
    { "Pale\nRose\nPink", 0xf39ec0, 37, 38, 39, 6},
    { "Salmon", 0xfb4525, 41, 40, 41, 6},
    { "Deep\nSalmon", 0xfb101e, 42, 42, 43, 6},
    { "Middle\nRose", 0xfc06a6, 44, 44, 45, 6},
    { "Light\nRose\nPurple", 0xbe31ab, 47, 46, 47, 6},
    { "Surprise\nPink", 0xdebcfd, 51, 48, 49, 6},
    { "No\nColor\nBlue", 0xc1cdfd, 60, 50, 51, 6},
    { "Cleanwater", 0xbec4fd, 360, 52, 53, 6},
    { "Booster\nBlue", 0x91c1fd, 62, 54, 55, 6},
    { "Tipton\nBlue", 0x97abfc, 362, 56, 57, 6},
    { "Blue\nBell", 0x7a94fc, 364, 58, 59, 6},
    { "Daylight\nBlue", 0x1296fb, 65, 60, 61, 6},
    { "Tharon\nDelft\nBlue", 0x7d88fb, 365, 62, 63, 6},
    { "Cerulean\nBlue", 0x16a6d6, 375, 64, 65, 6},
    { "Bermuda\nBlue", 0x0f74a1, 376, 66, 67, 6},
    { "Green\nBlue", 0x083b83, 77, 68, 69, 6},
    { "Alice\nBlue", 0x7058fb, 378, 70, 71, 6},
    { "Primary\nBlue", 0x0028fa, 80, 72, 73, 6},
    { "Baldassari\nBlue", 0x2900df, 381, 74, 75, 6},
    { "Medium\nBlue", 0x0000E6, 83, 76, 77, 6},
    { "Pale\nYellow\nGreen", 0xedffac, 87, 78, 79, 6},
    { "Light\nGreen", 0xd6ff80, 88, 80, 81, 6},
    { "Moss\nGreen", 0x49fb44, 89, 82, 83, 6},
    { "Primary\nGreen", 0x0a4e21, 91, 84, 85, 6},
    { "Turquoise", 0x2affb4, 92, 86, 87, 6},
    { "Blue\nGreen", 0x13938e, 93, 88, 89, 6},
    { "Chocolate", 0xc37452, 99, 90, 91, 6},
};

static const src_table_t Source_Table[] = 
{
    {"Tungsten\nBulb",{ 4260 ,4005  },},
    {"Incandescent",{ 4433 ,4077  },},
    {"Halogen",{ 4355 ,4059  },},
    {"Antique\nBulb",{ 5091 ,4225  },},
    {"Warm Antique Bulb",{ 5487 ,4139  },},
    {"Christmas\nLights",{ 4891 ,4198  },},
    {"Night\nLight",{ 5193 ,4133  },},
    {"Infrared\nHeat\nLamp",{ 6584 ,3128  },},
    {"Grow\nLight",{ 3735 ,4197  },},
    {"Cfl\nSoft\nWhite",{ 4695 ,4263  },},
    {"Cfl\nBright\nWhite",{ 4156 ,4090  },},
    {"Cfl\nCool\nWhite",{ 3963 ,3978  },},
    {"Cfl\nDaylight",{ 3100 ,3339  },},
    {"Cool\nWhite 1",{ 3716 ,3860  },},
    {"Cool\nWhite 2",{ 3854 ,3753  },},
    {"Cool\nWhite 3",{ 3484 ,3691  },},
    {"Warm\nWhite",{ 4509 ,4258  },},
    {"Cfl\nBlacklight",{ 1962 ,1115  },},
    {"Hmi",{ 3341 ,3415  },},
    {"High\nPressure\nSodium",{ 5242 ,4285  },},
    {"Low\nPressure\nSodium",{ 5410 ,4238  },},
    {"Mercury\nVapor",{ 3702 ,3875  },},
    {"Metal\nHalide",{ 3860 ,3951  },},
    {"Ceramic",{ 4097 ,3930  },},
    {"Carbon\nArc",{ 3350 ,3427  },},
    {"Xenon",{ 3242 ,3359  },},
    {"Candle",{ 5210 ,4158  },},
    {"Gas\nFire",{ 5000 ,4058  },},
    {"Sun\nDirect",{ 3361 ,3505  },},
    {"Sun\nOvercast",{ 3198 ,3338  },},
    {"Sun\nBlue\nHour",{ 2886 ,3069  },},
    {"Mobile\nPhone",{ 3002 ,3166  },},
    {"Computer\nMonitor",{ 3325 ,3428  },},
    {"Electrolumonence",{ 1871 ,4820  },},
    {"Blow\nTorch",{ 2115 ,2482  },},
    {"Road\nFlare",{ 6329 ,3289  },},
    {"Amber\nCaution",{ 5656 ,4040  },},
    {"Green\nTraffic\nLight",{ 2639 ,5563  },},
    {"Yellow\nTraffic\nLight",{ 5097 ,4477  },},
    {"Red Traffic\nLight",{ 6396 ,3287  },},
    {"Blue Glow\nStick",{ 1493 ,827   },},
    {"Green Glow\nStick",{ 2263 ,6655  },},
    {"Red Glow\nStick",{ 6920 ,3002  },},
    {"Yellow Glow\nStick",{ 4028 ,5347  },},
    {"Pink Glow\nStick",{ 3932 ,1886  },},
    {"Viole Glow\nStick",{ 2753 ,1345  },},
};

static const char* LEE_series_str[] = 
{
    [LEE_COLOR_CORRECTION] = "Col.Corr",
    [LEE_COLOR_FILTERS] = "Cal.Filter",
    [LEE_COLOR_600S] = "600 Series",
    [LEE_COLOR_COS_FILTERS] = "Cosmetic",
    [LEE_COLOR_700S] = "700 Series",    
};

static const char* ROSCO_series_str[] = 
{
    [ROSCO_COLOR_CORRECTION] = "Col.Corr",
    [ROSCO_COLOR_CALCOLOR] = "Cal Color",
    [ROSCO_STORARO_SEL] = "Storaro",
    [ROSCO_CINELUX] = "Cinelux",
};


static const char* dmx_profile_str[] = {
    #if PROJECT_TYPE==307
    "CCT & RGB 8-bit",
    "CCT 8-bit",
    "CCT & HSI 8-bit",
    "RGB 8-bit",
    "Adv.HSI 8-bit", 
	"GEL 8-bit",
	"XY 8-bit",
    "Source 8-bit",
    "FX 8-bit",
	"WWRGB 8-bit",
    "Ultimate 8-bit",
    "CCT & RGB 16-bit",
    "CCT 16-bit",
    "CCT & HSI 16-bit",
    "RGB 16-bit",
    "Adv.HSI 16-bit",   
	"GEL 16-bit",
    "XY 16-bit",
    "Source 16-bit",
    "FX 16-bit",
	"WWRGB 16-bit",
    "Ultimate 16-bit",
    #elif PROJECT_TYPE==308
    "CCT 8-bit",
    "FX 8-bit",
    "CCT & FX 8-bit",
    "CCT 16-bit",
    "FX 16-bit",
    "CCT & FX 16-bit",
    #endif
};

aputure_limit_angle_type  aputure_limit_angle_value[7] = {
	{-115, 115},
    {-100, 100},
    {-105, 105},
	{-110, 110},
    {-120, 120},
	{-115, 115},
    {-135, 135},
};
/*********************
 *  GLOBAL VATIABLES
 *********************/
UI_Data_t g_tUIAllData;
UI_State_Data_t g_tUIStateData;
UI_State_Data_t* g_ptUIStateData = &g_tUIStateData;
static uint8_t s_cfx_read_bank_state = 0;
static uint8_t s_ui_dmx_state_src = 0;
static uint8_t ctr_last_spid_index = 0;
static uint8_t s_hide_dropdown_flag = 0; 
static uint8_t s_ui_trigger_cfx = 0;
static uint8_t s_ui_fresnel_angle_flag = 0;
static uint8_t s_ui_updata_start_flag = 0;
static uint8_t s_ui_max_power_flag = 0;
static uint8_t s_ui_set_motor_reset_flag = 0;
static uint8_t s_work_get_update_flag = 0; 
static uint8_t s_ele_reset_flag = 0; 
static uint8_t s_batch_update_serial_num = 0;
static uint16_t s_ui_upgrade_timeout = 0;
/*********************
 *  GLOBAL FUNCTIONS
 *********************/
const gel_dsc_t* gel_get_serial_dsc(uint8_t brand, uint8_t serials)
{
    const gel_dsc_t* LEE_gel_dsc[] = {
        &LEE_ColorCorrection[0],
        &LEE_ColorFilters[0],
        &LEE_600Series[0],
        &LEE_CosmeticFilters[0],
        &LEE_700Series[0],
    };

    const gel_dsc_t* ROSCO_gel_dsc[] = {
        &ROSCO_ColorCorrection[0],
        &ROSCO_CalColor[0],
        &ROSCO_StoraroSelection[0],
        &ROSCO_Cinelux[0],
    };

    if (brand == LEE)
    {
        if (serials >= LEE_COLOR_MAX)
            serials = LEE_COLOR_MAX - 1;

        return LEE_gel_dsc[serials];
    }
    else
    {
        if (serials >= ROSCO_COLOR_MAX)
            serials = ROSCO_COLOR_MAX - 1;

        return ROSCO_gel_dsc[serials];
    }
}

const char* gel_get_serial_name(uint8_t brand, uint8_t serials)
{
    if (brand == LEE)
    {
        if (serials >= LEE_COLOR_MAX)
            serials = LEE_COLOR_MAX - 1;

        return LEE_series_str[serials];
    }
    else
    {
        if (serials >= ROSCO_COLOR_MAX)
            serials = ROSCO_COLOR_MAX - 1;

        return ROSCO_series_str[serials];
    }
}

uint8_t gel_get_max_num(uint8_t brand, uint8_t serials)
{
    const uint8_t gel_LEE_maxium[] = 
    {
        39,
        89,
        9,
        18,
        41
    };
    
    const uint8_t gel_ROSCO_maxium[] = 
    {
        33,
        33,
        10,
        46,
    };
    
    if (brand == LEE)
    {
        if (serials >= LEE_COLOR_MAX)
            serials = LEE_COLOR_MAX - 1;

        return gel_LEE_maxium[serials];
    }
    else
    {
        if (serials >= ROSCO_COLOR_MAX)
            serials = ROSCO_COLOR_MAX - 1;

        return gel_ROSCO_maxium[serials];
    } 
}

const src_table_t* source_get_table(uint8_t src_index)
{
    if (src_index >= sizeof(Source_Table) / sizeof(Source_Table[0]))
    {
        src_index = 0;
    }

    return &Source_Table[src_index];
}

const char* source_get_name(uint8_t src_index)
{
    const src_table_t *src = source_get_table(src_index);
    return src->name;
}

void ui_preset_task_del(uint8_t index)
{
    data_center_delete_preset_data(index);
}

void ui_preset_task_save(uint8_t index, preset_data_t* task)
{
    uint8_t id = screen_get_act_pid();
    void* user_data = screen_get_act_page_data();

    if (user_data == NULL)
        return;

    switch (id)
    {
        case PAGE_LIGHT_MODE_CCT:
        {
            task->mode = LIGHT_MODE_CCT;
            memcpy(&task->value, user_data, sizeof(struct db_cct));     
			
        }
        break;
        case PAGE_LIGHT_MODE_RGB:
        {
            task->mode = LIGHT_MODE_RGB;
            memcpy(&task->value, user_data, sizeof(struct db_rgb));                        
        }
        break;
        case PAGE_LIGHT_MODE_HSI:
        {
            task->mode = LIGHT_MODE_HSI;
            memcpy(&task->value, user_data, sizeof(struct db_hsi));    
        }
        break;
        case PAGE_LIGHT_MODE_XY:
        {
            task->mode = LIGHT_MODE_XY;
            memcpy(&task->value, user_data, sizeof(struct db_xy));
        }
        break;
        case PAGE_LIGHT_MODE_SOURCE:
        {            
            task->mode = LIGHT_MODE_SOURCE;
            memcpy(&task->value, user_data, sizeof(struct db_source));           
        }
        break;
        case PAGE_LIGHT_MODE_GEL:
        {
            task->mode = LIGHT_MODE_GEL;
            memcpy(&task->value, user_data, sizeof(struct db_gel));
        }
        break;
        case PAGE_PULSING:
        {
            #if (PROJECT_TYPE == 308)
            task->mode = LIGHT_MODE_FX_PULSING;
            memcpy(&task->value, user_data, sizeof(struct db_fx_pulsing));  
            #else
            task->mode = LIGHT_MODE_FX_PULSING_II;
            memcpy(&task->value, user_data, sizeof(struct db_fx_pulsing_2));  
            #endif
        }
        break;
        case PAGE_STROBE:
        {
            #if (PROJECT_TYPE == 308)
            task->mode = LIGHT_MODE_FX_STROBE;
            memcpy(&task->value, user_data, sizeof(struct db_fx_strobe)); 
            #else
            task->mode = LIGHT_MODE_FX_STROBE_II;
            memcpy(&task->value, user_data, sizeof(struct db_fx_strobe_2));  
            #endif
        }
        break;
        case PAGE_EXPLOSIOIN:
        {
            #if (PROJECT_TYPE == 308)
            task->mode = LIGHT_MODE_FX_EXPLOSION;
            memcpy(&task->value, user_data, sizeof(struct db_fx_explosion));            
            #else
            task->mode = LIGHT_MODE_FX_EXPLOSION_II;
            memcpy(&task->value, user_data, sizeof(struct db_fx_explosion_2));
            #endif
        }
        break;
        case PAGE_FAULTYBULB:
        {  
            #if (PROJECT_TYPE == 308)
            task->mode = LIGHT_MODE_FX_FAULT_BULB;
            memcpy(&task->value, user_data, sizeof(struct db_fx_fault_bulb));             
            #else
            task->mode = LIGHT_MODE_FX_FAULT_BULB_II;
            memcpy(&task->value, user_data, sizeof(struct db_fx_fault_bulb_2));   
            #endif
        }
        break;
        case PAGE_WELDING:
        {
            task->mode = LIGHT_MODE_FX_WELDING_II;
            memcpy(&task->value, user_data, sizeof(struct db_fx_welding_2));               
        }
        break;
        case PAGE_COPCAR:
        {
            task->mode = LIGHT_MODE_FX_COP_CAR_II;
            memcpy(&task->value, user_data, sizeof(struct db_fx_cop_car_2));                      
        }
        break;
        case PAGE_CANDLE:
        {
            task->mode = LIGHT_MODE_FX_CANDLE;
            memcpy(&task->value, user_data, sizeof(struct db_fx_candle));              
        }
        break;
        case PAGE_CLUBLIGHTS:
        {
            task->mode = LIGHT_MODE_FX_CLUBLIGHTS;
            memcpy(&task->value, user_data, sizeof(struct db_fx_club_lights));      
        }
        break;
        case PAGE_COLORCHASE:
        {
            task->mode = LIGHT_MODE_FX_COLOR_CHASE;
            memcpy(&task->value, user_data, sizeof(struct db_fx_color_chase));                    
        }
        break;
        case PAGE_TV:
        {    
            #if (PROJECT_TYPE == 308)
            task->mode = LIGHT_MODE_FX_TV;
            memcpy(&task->value, user_data, sizeof(struct db_fx_tv));              
            #else
            task->mode = LIGHT_MODE_FX_TV_II;
            memcpy(&task->value, user_data, sizeof(struct db_fx_tv_2));   
            #endif
        }
        break;
        case PAGE_PAPARAZZI:  //单色温和全彩做区分
        {
            task->mode = LIGHT_MODE_FX_PAPARAZZI;
            memcpy(&task->value, user_data, sizeof(struct db_fx_paparazzi));  
        }
        break;
        case PAGE_LIGHTNING:
        {
            #if (PROJECT_TYPE == 308)
            task->mode = LIGHT_MODE_FX_LIGHTNING;
            memcpy(&task->value, user_data, sizeof(struct db_fx_lightning));           
            #else
            task->mode = LIGHT_MODE_FX_LIGHTNING_II;
            memcpy(&task->value, user_data, sizeof(struct db_fx_lightning_2));
            #endif
        }
        break;
        case PAGE_FIREWORKS:
        {
            #if (PROJECT_TYPE == 308)
            task->mode = LIGHT_MODE_FX_FIREWORKS;
            memcpy(&task->value, user_data, sizeof(struct db_fx_fireworks));  
            #else
            task->mode = LIGHT_MODE_FX_FIREWORKS;
            memcpy(&task->value, user_data, sizeof(struct db_fx_fireworks));           
            #endif
        }
        break;
        case PAGE_FIRE:
        {
            #if (PROJECT_TYPE == 308)
            task->mode = LIGHT_MODE_FX_FIRE;
            memcpy(&task->value, user_data, sizeof(struct db_fx_fire));  
            #else
            task->mode = LIGHT_MODE_FX_FIRE_II;
            memcpy(&task->value, user_data, sizeof(struct db_fx_fire_2));   
            #endif            
        }
        break;
        case PAGE_PARTYLIGHT:
        {
            task->mode = LIGHT_MODE_FX_PARTY_LIGHTS;
            memcpy(&task->value, user_data, sizeof(struct db_fx_party_lights));            
        }
        break;
        default:break;
    }

    data_center_write_preset_data(index, task);            
}

void ui_preset_task_get(uint8_t index, preset_data_t* param)
{
    if (data_center_read_preset_data(index, param) != 0) {
        param->mode = LIGHT_MODE_FACTORY_PWM;
        return ;
    }

    if (!preset_task_light_mode_valid(param->mode)) {
        param->mode = LIGHT_MODE_FACTORY_PWM;
        return ;
    }
}

uint8_t ui_preset_task_apply(uint8_t index, preset_data_t *task)
{
    uint8_t page_id = PAGE_LIGHT_MODE_CCT;
//	void* user_data = screen_get_act_page_data();

//    if (user_data == NULL)
//			return 0;
    switch (task->mode)
    {
        case LIGHT_MODE_CCT:
        {
            page_id = PAGE_LIGHT_MODE_CCT;    
//			memcpy(user_data ,&task->value, sizeof(struct db_cct)); 
            memcpy(&g_tUIAllData.cct_model, &task->value, sizeof(struct db_cct));
			data_center_write_light_data(LIGHT_MODE_CCT, &task->value); 
        }
        break;
        case LIGHT_MODE_HSI:
        {
            page_id = PAGE_LIGHT_MODE_HSI;
			memcpy(&g_tUIAllData.hsi_model, &task->value, sizeof(struct db_hsi));
//			memcpy(user_data ,&task->value, sizeof(struct db_hsi));  
			data_center_write_light_data(LIGHT_MODE_HSI, &task->value); 
        }
        break;
        case LIGHT_MODE_RGB:
        {
            page_id = PAGE_LIGHT_MODE_RGB;
			memcpy(&g_tUIAllData.rgb_model, &task->value, sizeof(struct db_rgb));
//			memcpy(user_data ,&task->value, sizeof(struct db_rgb)); 
           data_center_write_light_data(LIGHT_MODE_RGB, &task->value); 
        }
        break;
        case LIGHT_MODE_GEL:
        {
            page_id = PAGE_LIGHT_MODE_GEL;
			memcpy(&g_tUIAllData.gel_model, &task->value, sizeof(struct db_gel));
//			memcpy(user_data ,&task->value, sizeof(struct db_gel));    
			data_center_write_light_data(LIGHT_MODE_GEL, &task->value); 
        }
        break;
        case LIGHT_MODE_XY:
        {
            page_id = PAGE_LIGHT_MODE_XY;
			memcpy(&g_tUIAllData.xy_model, &task->value, sizeof(struct db_xy));
//			memcpy(user_data ,&task->value, sizeof(struct db_xy));  
			data_center_write_light_data(LIGHT_MODE_XY, &task->value); 
        }
        break;
        case LIGHT_MODE_SOURCE:
        {
            page_id = PAGE_LIGHT_MODE_SOURCE;
			memcpy(&g_tUIAllData.source_model, &task->value, sizeof(struct db_source));
//			memcpy(user_data ,&task->value, sizeof(struct db_source)); 
            data_center_write_light_data(LIGHT_MODE_SOURCE, &task->value);            
        }
        break;
        case LIGHT_MODE_FX_PULSING:
        {
            page_id = PAGE_PULSING;
			#if PROJECT_TYPE==307
			memcpy(&g_tUIAllData.pulsing2_model, &task->value, sizeof(struct db_fx_pulsing_2));
			#elif PROJECT_TYPE==308
			memcpy(&g_tUIAllData.pulsing_model, &task->value, sizeof(struct db_fx_pulsing));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_pulsing)); 
            data_center_write_light_data(LIGHT_MODE_FX_PULSING, &task->value);          
        }
        break;
        case LIGHT_MODE_FX_PULSING_II:
        {
            page_id = PAGE_PULSING;
			memcpy(&g_tUIAllData.pulsing2_model, &task->value, sizeof(struct db_fx_pulsing_2));
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_pulsing_2)); 
            data_center_write_light_data(LIGHT_MODE_FX_PULSING_II, &task->value);          
        }
        break;
        case LIGHT_MODE_FX_STROBE:
        {
            page_id = PAGE_STROBE;
			#if PROJECT_TYPE==307
			memcpy(&g_tUIAllData.strobe2_model, &task->value, sizeof(struct db_fx_strobe_2));
			#elif PROJECT_TYPE==308
			memcpy(&g_tUIAllData.strobe_model, &task->value, sizeof(struct db_fx_strobe));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_strobe)); 
            data_center_write_light_data(LIGHT_MODE_FX_STROBE, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_STROBE_II:
        {
            page_id = PAGE_STROBE;
			#if PROJECT_TYPE==307
			memcpy(&g_tUIAllData.strobe2_model, &task->value, sizeof(struct db_fx_strobe_2));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_strobe_2)); 
            data_center_write_light_data(LIGHT_MODE_FX_STROBE_II, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_EXPLOSION:
        {
            page_id = PAGE_EXPLOSIOIN;
			memcpy(&g_tUIAllData.explosion_model, &task->value, sizeof(struct db_fx_explosion));
			task->value.explosion.trigger = 0;
			task->value.explosion.state = 0;
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_explosion)); 
            data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_EXPLOSION_II:
        {
            page_id = PAGE_EXPLOSIOIN;
			#if PROJECT_TYPE==307
			memcpy(&g_tUIAllData.explosion2_model, &task->value, sizeof(struct db_fx_explosion_2));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_explosion_2)); 
            data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION_II, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_FAULT_BULB:
        {
            page_id = PAGE_FAULTYBULB;
			memcpy(&g_tUIAllData.faultybulb_model, &task->value, sizeof(struct db_fx_fault_bulb));
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_fault_bulb)); 
            data_center_write_light_data(LIGHT_MODE_FX_FAULT_BULB, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_FAULT_BULB_II:
        {
            page_id = PAGE_FAULTYBULB;
			#if PROJECT_TYPE==307
			memcpy(&g_tUIAllData.faultybulb2_model, &task->value, sizeof(struct db_fx_fault_bulb_2));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_fault_bulb_2)); 
            data_center_write_light_data(LIGHT_MODE_FX_FAULT_BULB_II, &task->value);  
        }
        break;
		case LIGHT_MODE_FX_WELDING:
		  {
            page_id = PAGE_WELDING;
			memcpy(&g_tUIAllData.welding2_model, &task->value, sizeof(struct db_fx_welding_2));
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_welding_2)); 
            data_center_write_light_data(LIGHT_MODE_FX_WELDING, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_WELDING_II:
        {
            page_id = PAGE_WELDING;
			memcpy(&g_tUIAllData.welding2_model, &task->value, sizeof(struct db_fx_welding_2));
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_welding_2)); 
            data_center_write_light_data(LIGHT_MODE_FX_WELDING_II, &task->value);  
        }
        break;
		
		 case LIGHT_MODE_FX_COP_CAR:
        {
            page_id = PAGE_COPCAR;
			memcpy(&g_tUIAllData.copcar2_model, &task->value, sizeof(struct db_fx_cop_car));
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_cop_car)); 
            data_center_write_light_data(LIGHT_MODE_FX_COP_CAR, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_COP_CAR_II:
        {
            page_id = PAGE_COPCAR;
			memcpy(&g_tUIAllData.copcar2_model, &task->value, sizeof(struct db_fx_cop_car));
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_cop_car_2)); 
            data_center_write_light_data(LIGHT_MODE_FX_COP_CAR_II, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_CANDLE:
        {
            page_id = PAGE_CANDLE;
			memcpy(&g_tUIAllData.candle_model, &task->value, sizeof(struct db_fx_candle));
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_candle)); 
            data_center_write_light_data(LIGHT_MODE_FX_CANDLE, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_CLUBLIGHTS:
        {
            page_id = PAGE_CLUBLIGHTS;
			memcpy(&g_tUIAllData.clublights_model, &task->value, sizeof(struct db_fx_club_lights));
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_club_lights)); 
            data_center_write_light_data(LIGHT_MODE_FX_CLUBLIGHTS, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_COLOR_CHASE:
        {
            page_id = PAGE_COLORCHASE;
			memcpy(&g_tUIAllData.colorchase_model, &task->value, sizeof(struct db_fx_color_chase));
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_color_chase)); 
            data_center_write_light_data(LIGHT_MODE_FX_COLOR_CHASE, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_TV:
        {
            page_id = PAGE_TV;
			#if PROJECT_TYPE==307
			memcpy(&g_tUIAllData.tv2_model, &task->value, sizeof(struct db_fx_tv_2));
			#elif PROJECT_TYPE==308
			memcpy(&g_tUIAllData.tv_model, &task->value, sizeof(struct db_fx_tv));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_tv)); 
            data_center_write_light_data(LIGHT_MODE_FX_TV, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_TV_II:
        {
            page_id = PAGE_TV;
			#if PROJECT_TYPE==307
			memcpy(&g_tUIAllData.tv2_model, &task->value, sizeof(struct db_fx_tv_2));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_tv_2)); 
            data_center_write_light_data(LIGHT_MODE_FX_TV_II, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_PAPARAZZI:
        {
            page_id = PAGE_PAPARAZZI;
			memcpy(&g_tUIAllData.paparazzi_model, &task->value, sizeof(struct db_fx_paparazzi));
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_paparazzi)); 
            data_center_write_light_data(LIGHT_MODE_FX_PAPARAZZI, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_LIGHTNING:
        {
            page_id = PAGE_LIGHTNING;
			#if PROJECT_TYPE==307
			if(task->value.lightning_2.trigger == 1)
				task->value.lightning_2.trigger = 0;
			memcpy(&g_tUIAllData.lightning2_model, &task->value, sizeof(struct db_fx_lightning_2));
			#elif PROJECT_TYPE==308
			if(task->value.lightning.trigger)
				task->value.lightning.trigger = 0;
			memcpy(&g_tUIAllData.lightning_model, &task->value, sizeof(struct db_fx_lightning));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_lightning)); 
            data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_LIGHTNING_II:
        {
            page_id = PAGE_LIGHTNING;
			#if PROJECT_TYPE==307
			memcpy(&g_tUIAllData.lightning2_model, &task->value, sizeof(struct db_fx_lightning_2));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_lightning_2)); 
            data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING_II, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_FIREWORKS:
        {
            page_id = PAGE_FIREWORKS;
			#if PROJECT_TYPE==307
			memcpy(&g_tUIAllData.fireworks_model, &task->value, sizeof(struct db_fx_fireworks));
			#elif PROJECT_TYPE==308
			memcpy(&g_tUIAllData.fireworks_model, &task->value, sizeof(struct db_fx_fireworks));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_fireworks)); 
            data_center_write_light_data(LIGHT_MODE_FX_FIREWORKS, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_FIREWORKS_II:
        {
           // page_id = PAGE_FIREWORKS;
			//#if PROJECT_TYPE==307
			//memcpy(&g_tUIAllData.fireworks_model, &task->value, sizeof(struct db_fx_fireworks_2));
			//#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_fireworks_2)); 
            //data_center_write_light_data(LIGHT_MODE_FX_FIREWORKS_II, &task->value);  
        }
        break;
        case LIGHT_MODE_FX_FIRE:
        {
            page_id = PAGE_FIRE;
			#if PROJECT_TYPE==307
			memcpy(&g_tUIAllData.fire2_model, &task->value, sizeof(struct db_fx_fire_2));
			#elif PROJECT_TYPE==308
			memcpy(&g_tUIAllData.fire_model, &task->value, sizeof(struct db_fx_fire));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_fire)); 
            data_center_write_light_data(LIGHT_MODE_FX_FIRE, &task->value); 
        }
        break;
        case LIGHT_MODE_FX_FIRE_II:
        {
            page_id = PAGE_FIRE;
			#if PROJECT_TYPE==307
			memcpy(&g_tUIAllData.fire2_model, &task->value, sizeof(struct db_fx_fire_2));
			#endif
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_fire_2)); 
            data_center_write_light_data(LIGHT_MODE_FX_FIRE_II, &task->value); 
        }
        break;
        case LIGHT_MODE_FX_PARTY_LIGHTS:
        {
            page_id = PAGE_PARTYLIGHT;
			memcpy(&g_tUIAllData.partylight_model, &task->value, sizeof(struct db_fx_party_lights));
//			memcpy(user_data ,&task->value, sizeof(struct db_fx_party_lights)); 
            data_center_write_light_data(LIGHT_MODE_FX_PARTY_LIGHTS, &task->value); 
        }
        break;
        default:break;
    }

    return page_id;
}

uint8_t ui_param_setting_get_page(param_setting_type_t type)
{
    uint8_t page_id = PAGE_CCT_SETTING;

    switch(type)
    {
        case PARAM_SETTING_CCT:     page_id = PAGE_CCT_SETTING;break;
        case PARAM_SETTING_HSI:     page_id = PAGE_HSI_SETTING;break;
        case PARAM_SETTING_RGB:     page_id = PAGE_RGB_SETTING;break;
        case PARAM_SETTING_GEL:     page_id = PAGE_GEL_SETTING;break;
        case PARAM_SETTING_SOURCE:  page_id = PAGE_SOURCE_SETTING;break;
        case PARAM_SETTING_XY:      page_id = PAGE_XY_SETTING;break;
    }   

    return page_id;
}

void ui_set_param_setting(param_setting_type_t type, struct db_fx_mode_arg* setting)
{
    uint8_t page_id = PAGE_CCT_SETTING;
    switch(type)
    {
        case PARAM_SETTING_CCT:     page_id = PAGE_CCT_SETTING;break;
        case PARAM_SETTING_HSI:     page_id = PAGE_HSI_SETTING;break;
        case PARAM_SETTING_RGB:     page_id = PAGE_RGB_SETTING;break;
        case PARAM_SETTING_GEL:     page_id = PAGE_GEL_SETTING;break;
        case PARAM_SETTING_SOURCE:  page_id = PAGE_SOURCE_SETTING;break;
        case PARAM_SETTING_XY:      page_id = PAGE_XY_SETTING;break;
    }

    page_set_user_data(page_id, (void *)setting);
}

void ui_set_limit_setting(param_setting_type_t type, struct db_fx_mode_limit_arg* setting)
{
    uint8_t page_id = PAGE_CCT_SETTING;
    switch(type)
    {
        case PARAM_SETTING_CCT:     page_id = PAGE_CCT_LIMIT_SETTING;break;
        case PARAM_SETTING_HSI:     page_id = PAGE_HSI_LIMIT_SETTING;break;
    }

    page_set_user_data(page_id, (void *)setting);
}

uint8_t ui_limit_setting_get_page(param_setting_type_t type)
{
    uint8_t page_id = PAGE_CCT_LIMIT_SETTING;    
    switch (type)
    {
    case PARAM_SETTING_CCT:    page_id = PAGE_CCT_LIMIT_SETTING;break;
    case PARAM_SETTING_HSI:    page_id = PAGE_HSI_LIMIT_SETTING;break;
    }

    return page_id;
}


const char* dmx_profile_get_name(int8_t type)
{
    #if PROJECT_TYPE==307
    if (type >= DMX_PROFILE_NUMS)
    {
        type = 0;
    }
    #elif PROJECT_TYPE==308
    if (type >= 6)
    {
        type = 0;
    }
    #endif
    return dmx_profile_str[type];
}

const char* ui_get_custom_fx_name(uint8_t type)
{
    const char* str = NULL;
    switch(type)
    {
        case CUSTOM_FX_TYPE_PIKER:  str = Lang_GetStringByID(STRING_ID_PIKER_FX); break;
        case CUSTOM_FX_TYPE_TOUCHBAR: str = Lang_GetStringByID(STRING_ID_MUSIC_FX); break;
        case CUSTOM_FX_TYPE_MUSIC:    str = Lang_GetStringByID(STRING_ID_MUSIC_FX); break;
    }
    return str;
}

/**
 * @brief 根据品牌获取色纸模型系列.
 * 
 * @param p_gel_model 
 * @param brand  ROSCO 或者 LEE
 * @return uint8_t 
 */
uint8_t ui_get_gel_series(struct db_gel* p_gel_model, uint8_t brand)
{
    uint8_t series = p_gel_model->gel.type[brand];
    uint8_t max_series = brand == ROSCO ? ROSCO_SUMS : LEE_SUMS;

    if (series >= max_series)
        series = max_series - 1;

    return series;
}

/**
 * @brief 根据品牌设置色纸模型的系列
 * 
 * @param p_gel_model 
 * @param brand  ROSCO 或者 LEE
 * @param series 
 *        ROSCO -> series < ROSCO_SUMS
 *        LEE   -> series < LEE_SUMS
 */
void ui_set_gel_series(struct db_gel* p_gel_model, uint8_t brand, uint8_t series)
{
    uint8_t max_series = brand == ROSCO ? ROSCO_SUMS : LEE_SUMS;  

    if (series >= max_series)
    {
        return ;
    }

    ui_enter_critical();
    p_gel_model->gel.type[brand] = series;
    ui_exit_critical();     
}

// void ui_set_gel_series(page_gel_t* p_gel_model, uint8_t series)
// {
//     uint8_t max_series = p_gel_model->brand_type == ROSCO ? ROSCO_SUMS : LEE_SUMS;

//     if (series >= max_series)
//     {
//         return ;
//     }

//     ui_enter_critical();
//     p_gel_model->series[p_gel_model->brand_type] = series;
//     ui_exit_critical(); 
// }

// void ui_set_gel_series(uint8_t brand, uint8_t series)
// {
//     uint8_t max_series = brand == ROSCO ? ROSCO_SUMS : LEE_SUMS;
//     page_gel_t* p_gel_model = &g_tUIAllData.gel_model;

//     if (series >= max_series)
//     {
//         return ;
//     }

//     ui_enter_critical();
//     p_gel_model->series[] = series;
//     ui_exit_critical();
// }

/**
 * @brief 设置GEL数据模型的品牌
 * 
 * @param p_gel_model 
 * @param brand     ROSCO 或者 LEE
 */
void ui_set_gel_brand(struct db_gel* p_gel_model, uint8_t brand)
{
    ui_enter_critical();
    p_gel_model->gel.brand = brand;
    ui_exit_critical();
}

/**
 * @brief 获取GEL数据模型的品牌
 * 
 * @param p_gel_model 
 * @return uint8_t 
 */
uint8_t ui_get_gel_brand(struct db_gel* p_gel_model)
{
    return p_gel_model->gel.brand;
}

/**
 * @brief 根据品牌/系列来设置GEL数据模型的色纸选项
 * 
 * @param p_gel_model 
 * @param brand     ROSCO 或者 LEE
 * @param series    
 *          ROSCO -> series < ROSCO_SUMS
 *          LEE   -> series < LEE_SUMS
 * @param index  色纸索引
 * @return uint8_t 
 */
void ui_set_gel_index(struct db_gel* p_gel_model, uint8_t brand, uint8_t series, uint8_t index)
{
    uint8_t max_series = brand == ROSCO ? ROSCO_SUMS : LEE_SUMS;
    uint8_t max_nums = gel_get_max_num(brand, series);

    UI_PRINTF("RealGel Index: %d ExceptGel Index: %d\r\n", p_gel_model->gel.color[brand][series], index);

    if (brand >= GEL_BRAND_MAX || series >= max_series || index >= max_nums || 
            p_gel_model->gel.color[brand][series] == index)
    {
        return ;
    }

    ui_enter_critical();
    p_gel_model->gel.color[brand][series] = index;        
    ui_exit_critical();    
}

/**
 * @brief 根据品牌和系列获取色纸索引
 * 
 * @param p_gel_model GEL数据模型
 * @param brand  品牌 ROSCO 或者 LEE
 * @param series 系列
 *          ROSCO -> series < ROSCO_SUMS
 *          LEE   -> series < LEE_SUMS
 */
uint8_t ui_get_gel_index(struct db_gel* p_gel_model, uint8_t brand, uint8_t series)
{
    uint8_t max_series = brand == ROSCO ? ROSCO_SUMS : LEE_SUMS;
    uint8_t max_nums = gel_get_max_num(brand, series);

    if (brand >= GEL_BRAND_MAX || series >= max_series)
    {
        return 0;
    }

    return p_gel_model->gel.color[brand][series];    
}

pm_data_t* ui_get_pm_data(void)
{
    return &g_tUIAllData.pm_data_model;
}

bool ui_get_master_mode(void)
{
	struct sys_info_dmx          dmx_link_state;
	data_center_read_sys_info(SYS_INFO_DMX, &dmx_link_state);
	if(dmx_link_state.eth_linked != 1 && dmx_link_state.wired_linked != 1 && dmx_link_state.wireless_linked != 1)
	{
		return g_tUIAllData.sys_menu_model.work_mode;
	}
	else
	{
		return 0;
	}
}

void ui_set_master_mode(uint8_t type)
{
    g_tUIAllData.sys_menu_model.work_mode= type;
}

bool ui_get_dmx_temination_mode(void)
{
    return g_tUIAllData.sys_menu_model.dmx_temination_on;
}

void ui_set_dmx_temination_mode(uint8_t state)
{
    g_tUIAllData.sys_menu_model.dmx_temination_on = state;
}

uint8_t ui_get_fan_mode(void)
{
    return g_tUIAllData.sys_menu_model.fan_mode;
}

void ui_set_fan_mode(uint8_t type)
{
    g_tUIAllData.sys_menu_model.fan_mode = type;
}

uint8_t ui_get_curve_type(void)
{
    return g_tUIAllData.sys_menu_model.curve_type;
}

void ui_set_curve_type(uint8_t type)
{
    g_tUIAllData.sys_menu_model.curve_type = type;
}

uint8_t ui_get_hight_speed_mode(void)
{
    return g_tUIAllData.sys_menu_model.hs_mode;
}

void ui_set_hight_speed_mode(uint8_t type)
{
    g_tUIAllData.sys_menu_model.hs_mode = type;
}

void ui_set_work_mode(uint8_t type)
{
    g_tUIAllData.sys_menu_model.work_mode = type;
}

uint8_t *ui_get_ble_sn(void)
{
    return g_tUIAllData.sys_menu_model.ble_sn_number;
}

uint8_t ui_get_comm_type(void)
{
    /* 刷新通信类型状态 */
    if (g_ptUIStateData->lan_state == true)
    {
        g_ptUIStateData->title.com_type = COM_TYPE_NETWORK;
    }
    else if (g_tUIAllData.sys_menu_model.ble_on == true)
    {
        g_ptUIStateData->title.com_type = COM_TYPE_BLE;   
    }
    else if (g_tUIAllData.sys_menu_model.crmx_on == true)
    {
        g_ptUIStateData->title.com_type = COM_TYPE_WIFI;
    }

    return g_ptUIStateData->title.com_type;
}

void ui_set_lan_state(uint8_t state)
{
    g_ptUIStateData->lan_state = state;
}

void ui_set_ble_state(uint8_t state)
{
    g_tUIAllData.sys_menu_model.ble_on = state;
}

void ui_set_crmx_state(uint8_t state)
{
    g_tUIAllData.sys_menu_model.crmx_on = state;
}

void ui_set_box_type(uint8_t state )
{
	g_ptUIStateData->box_type = state;
}

void ui_get_dhcp_ip_addr(uint8_t *addr)
{
	uint32_t ip_addr = netif_get_dhcp_ip_addr();
	addr[3] = (ip_addr & 0xff000000) >> 24;
	addr[2] = (ip_addr & 0x00ff0000) >> 16;
	addr[1] = (ip_addr & 0x0000ff00) >> 8;
	addr[0] = (ip_addr & 0x000000ff);
}

void ui_get_dhcp_netmask_addr(uint8_t *addr)
{
	uint32_t ip_addr = netif_get_dhcp_netmask();
	addr[3] = (ip_addr & 0xff000000) >> 24;
	addr[2] = (ip_addr & 0x00ff0000) >> 16;
	addr[1] = (ip_addr & 0x0000ff00) >> 8;
	addr[0] = (ip_addr & 0x000000ff);
}

void ui_get_dhcp_gateway_addr(uint8_t *addr)
{
	uint32_t ip_addr = netif_get_dhcp_gateway_addr();
	addr[3] = (ip_addr & 0xff000000) >> 24;
	addr[2] = (ip_addr & 0x00ff0000) >> 16;
	addr[1] = (ip_addr & 0x0000ff00) >> 8;
	addr[0] = (ip_addr & 0x000000ff);
}

void ui_get_mac_addr(void)
{
	uint8_t mac[6];
	
	netif_get_mac_addr(mac);
	
	g_ptUIStateData->mac_address[0] = mac[0];
	g_ptUIStateData->mac_address[1] = mac[1];
	g_ptUIStateData->mac_address[2] = mac[2];
	g_ptUIStateData->mac_address[3] = mac[3];
	g_ptUIStateData->mac_address[4] = mac[4];
	g_ptUIStateData->mac_address[5] = mac[5];
}

void ui_set_crmx_circle_state(uint8_t state)
{
	switch(state)
    {
        case 0:		g_ptUIStateData->lumen_status.status = LUMENRADIO_NO_PAIR;break;
        case 1: 	break;
        case 2:		g_ptUIStateData->lumen_status.status = LUMENRADIO_PAIRING;break;                     
        case 3:   	g_ptUIStateData->lumen_status.status = LUMENRADIO_PARIED_SUCCEED;break;   
        case 4:   	g_ptUIStateData->lumen_status.status = LUMENRADIO_PARIED_FAILED;break;                               
        default: break;
    }
}

void ui_set_power_calibration_state(uint8_t state)
{
	g_ptUIStateData->test_data.CalibrateState = state;
}

uint8_t ui_get_power_calibration_state(void)
{
	return g_ptUIStateData->test_data.CalibrateState;
}

void ui_set_single_power_calibration_state(uint8_t state)
{
	g_ptUIStateData->test_data.powercalibrateState = state;
}

uint8_t ui_get_single_power_calibration_state(void)
{
	return g_ptUIStateData->test_data.powercalibrateState;
}

void ui_set_power(uint16_t power)
{
	g_ptUIStateData->test_data.CalibratePower = power;
	g_ptUIStateData->test_data.SupplyPower = power;
	g_ptUIStateData->test_data.powercalibratePower = power;
}

uint8_t ui_get_power_type(void)
{
    return  g_ptUIStateData->title.power_type;
}

void ui_set_power_type(uint8_t state)
{
	g_ptUIStateData->title.power_type = state;
}


uint8_t ui_get_lang_type(void)
{
    return g_tUIAllData.sys_menu_model.lang;
}

void ui_set_lang_type(uint8_t lang)
{
    g_tUIAllData.sys_menu_model.lang = lang;
}

bool ui_data_version_check(UI_Data_t* pdata)
{
    if (pdata->magic_number != UI_DATA_MAGIC_NUMBER)
    {
        return false;
    }    

    return true;
}

void ui_test_motor_test_set(uint8_t mode)
{
	g_ptUIStateData->test_data.motor_test_mode = mode;
}

uint8_t ui_test_motor_test_get(void)
{
	return g_ptUIStateData->test_data.motor_test_mode;
}

void ui_test_rgbpwm_mode_set(uint8_t mode)
{
	g_ptUIStateData->test_data.PwmMode = mode;
}

uint8_t ui_test_rgbpwm_mode_get(void)
{
	return g_ptUIStateData->test_data.PwmMode;
}

void ui_test_power_mode_set(uint8_t mode)
{
	g_ptUIStateData->test_data.power_set_mode = mode;
}

uint8_t ui_test_power_value_get(void)
{
	return g_ptUIStateData->test_data.power_set_value;
}

uint8_t ui_test_power_mode_get(void)
{
	return g_ptUIStateData->test_data.power_set_mode;
}


uint8_t ui_get_dmx_state(void)
{
	return s_ui_dmx_state_src;
}

void ui_set_dmx_state(uint8_t state)
{
	s_ui_dmx_state_src = state;
}

void ui_set_hs_mode_state(uint8_t state)
{
	g_tUIAllData.sys_menu_model.hs_mode = state;
}

uint8_t ui_get_hs_mode_state(void)
{
	return g_tUIAllData.sys_menu_model.hs_mode;
}


void ui_test_data_set(void)
{
	struct sys_info_lamp   lamp_info = {0};
	struct sys_info_ctrl_box   box_info = {0};
	struct sys_info_motor_test   motor_test_info = {0};
	data_center_read_sys_info(SYS_INFO_LAMP, &lamp_info);
	data_center_read_sys_info(SYS_INFO_CTRL, &box_info);
	data_center_read_sys_info(SYS_INFO_MOTOR_TEST, &motor_test_info);
	g_ptUIStateData->test_data.LampNTCTemp =  lamp_info.cob_temp ;
	g_ptUIStateData->test_data.LampFanRealRPM = lamp_info.fan_speed;	
	g_ptUIStateData->test_data.FanRealRPM = box_info.fan_speed;
	g_ptUIStateData->test_data.motor_test[1] = motor_test_info.motor_test_count;
}

void ui_test_data_init(void)
{
	g_ptUIStateData->test_data.LampFanAdjRPM = 2500;
	g_ptUIStateData->test_data.FanAdjRPM = 2000;  
	g_ptUIStateData->test_data.pump_speed = 7000;

}

uint16_t ui_test_motor_state_get(void)
{
	return g_ptUIStateData->test_data.motor_test[0] ;
}

uint16_t ui_test_motor_count_get(void)
{
	return g_ptUIStateData->test_data.motor_test[1] ;
}

uint16_t ui_test_lamp_pump_speed_get(void)
{
	return g_ptUIStateData->test_data.pump_speed ;
}

uint8_t ui_test_lamp_pump_mode_get(void)
{
	return g_ptUIStateData->test_data.PumpMode ;
}

uint16_t ui_test_lamp_fan_get(void)
{
	return g_ptUIStateData->test_data.LampFanAdjRPM ;
}

uint16_t ui_test_box_fan_get(void)
{
	return g_ptUIStateData->test_data.FanAdjRPM ;
}

uint8_t ui_test_Industrial_mode_get(void)
{
	return g_ptUIStateData->test_data.IndustrialMode ;
}

uint8_t ui_get_cfx_read_bank_state(void)
{
    return s_cfx_read_bank_state;
}

void ui_set_cfx_read_bank_state(uint8_t state)
{
    s_cfx_read_bank_state =  state;
}

uint8_t ui_get_eth_universe_state(void)
{
    return g_ptUIStateData->eth_universe_state;
}

void ui_set_eth_universe_state(uint8_t state)
{
    g_ptUIStateData->eth_universe_state =  state;
	if(state == ETH_UNIVERSE_SACN)
	{
		artnet_data_state_set(0);
		sacn_data_state_set(1);
	}
	else if(state == ETH_UNIVERSE_ARTNET)
	{
		artnet_data_state_set(1);
		sacn_data_state_set(0);
	}
	else if(state == ETH_UNIVERSE_AUTOMATIC)
	{
		artnet_data_state_set(1);
		sacn_data_state_set(1);
	}
	else if(state == ETH_UNIVERSE_OFF)
	{
		artnet_data_state_set(0);
		sacn_data_state_set(0);
	}
}

uint8_t ui_get_eth_state(void)
{
    return g_ptUIStateData->eth_state;
}

void ui_set_eth_state(uint8_t state)
{
    g_ptUIStateData->eth_state =  state;
}

uint16_t ui_get_eth_artnet_universe(void)
{
    return g_ptUIStateData->eth_artnet_universe;
}

void ui_set_eth_artnet_universe(uint16_t universe)
{
    g_ptUIStateData->eth_artnet_universe =  universe;
	artnet_set_universe_address(universe);
}

uint16_t ui_get_eth_sacn_universe(void)
{
    return g_ptUIStateData->eth_sacn_universe;
}

uint8_t ui_get_high_speed_switch(void)
{
	if(date_center_get_light_mode() >= LIGHT_MODE_FX_CLUBLIGHTS && date_center_get_light_mode() < LIGHT_MODE_DMX)
	{
			return 1;
	}
	else if(g_ptUIStateData->high_speed_switch && date_center_get_light_mode() == LIGHT_MODE_DMX)
	{
		return 2;
	}
    return 0;
}

void ui_set_high_speed_switch(bool state)
{
    g_ptUIStateData->high_speed_switch = state;
}

void ui_set_eth_sacn_universe(uint16_t universe)
{
    g_ptUIStateData->eth_sacn_universe =  universe;
	sacn_set_universe(universe);
}

void ui_set_Lightning_2_state(uint8_t state)
{
    g_ptUIStateData->trigger_light_anim  =  state;
	if(g_ptUIStateData->trigger_light_anim == 1)
		g_ptUIStateData->trigger_light_anim_state = 1;
}

void ui_set_explosion_2_anim(uint8_t state)
{
    g_ptUIStateData->trigger_explosion_anim  =  state;
	if(g_ptUIStateData->trigger_explosion_anim == 1)
		g_ptUIStateData->trigger_explosion_anim_state = 1;
}

uint8_t get_lfm_port_mode(void)
{
    return g_tUIAllData.sys_menu_model.work_mode;
}


void ui_set_explosion_2_state(uint8_t state)
{
	#if (PROJECT_TYPE == 307)
      g_tUIAllData.explosion2_model.state = state;
	#endif
}
uint8_t ui_set_explosion_2_state_get(void)
{
	return g_ptUIStateData->trigger_explosion_state;
}

void ui_set_lightning_2_trigger(uint8_t trigger)
{
	#if (PROJECT_TYPE == 307)
	  g_tUIAllData.lightning2_model.trigger = trigger;
	#endif
}


void ui_get_pwm_rgbww(uint16_t data[])
{
	memcpy(data,g_ptUIStateData->test_data.rgbww,sizeof(g_ptUIStateData->test_data.rgbww));
}

uint8_t ui_get_pwm_PwmMode(void)
{
	return g_ptUIStateData->test_data.PwmMode;
}

void gui_ble_reset_event_gen(void)
{
	if (g_tUIAllData.sys_menu_model.work_mode == 1)
	{
		lfm_core_add_event_flag(LFM_EVENT_BLE_RESET);
	}	
}

void gui_factory_event_gen(void)
{
	if (g_tUIAllData.sys_menu_model.work_mode == 1)
	{
		lfm_core_add_event_flag(LFM_EVENT_FACTORY_RESET);
	}	
}

void gui_data_sync_event_gen(void)
{
	if (g_tUIAllData.sys_menu_model.work_mode == 1)
	{
		lfm_core_add_event_flag(LFM_EVENT_DATA_SYNC);
	}	
}

void gui_power_event_gen(void)
{
	if (g_tUIAllData.sys_menu_model.work_mode == 1)
	{
		lfm_core_add_event_flag(LFM_EVENT_POWER);
	}	
}

void gui_electric_move_event_gen(void)
{
	if (g_tUIAllData.sys_menu_model.work_mode == 1)
	{
		lfm_core_add_event_flag(LFM_EVENT_ELECTRIC_MOVE);
	}	
}

void gui_lfm_seng_data_set_flag(uint8_t flag)
{
	lfm_send_light_data_set_flag(flag);
}

uint8_t  gui_lfm_seng_data_get_flag(void)  
{
	return lfm_send_light_data_get_flag();	
}


uint64_t gui_get_64type_convert(uint8_t value)
{
	uint64_t data1 = 0;
	uint64_t data2 = 1;
	
	data1 = data2 << value;
	
	return data1;
}

void gui_page_sync_event_gen(void)
{
	if (g_tUIAllData.sys_menu_model.work_mode == 1)
	{
		lfm_core_add_event_flag(LFM_EVENT_PAGE_SYNC);
	}		
}

void gui_crmx_unpair_event_gen(void)
{
	if (g_tUIAllData.sys_menu_model.work_mode == 1)
	{
		lfm_core_add_event_flag(LFM_EVENT_CRMX_UNPAIR);
	}			
}

uint8_t gui_get_indoor_flag(void)
{
	struct sys_info_power   power_info = {0};
	data_center_read_sys_info(SYS_INFO_POWER, &power_info);

	return power_info.indoor_powe;
}

static int16_t s_power_limit = 1700;

uint16_t gui_get_power_limit(void)
{
	return s_power_limit;
}

uint16_t gui_set_power_limit(uint16_t power)
{
	s_power_limit = power;
}

//如果定义FreeRTOS
#ifdef UI_PLATFORM_MCU
#include "cmsis_os2.h"
#endif

void ui_enter_critical(void)
{
#ifdef UI_PLATFORM_MCU
    osKernelLock();
#endif
}

void ui_exit_critical(void)
{
#ifdef UI_PLATFORM_MCU
    osKernelUnlock();
#endif    
}

#define PARAM(param, type)      (param)        
void ui_reset_hsi(struct db_hsi *p_args)
{    
    PARAM(p_args, struct db_hsi)->hsi.cct = 5600;
    PARAM(p_args, struct db_hsi)->hsi.hue = 100;
    PARAM(p_args, struct db_hsi)->hsi.sat = 50;
    PARAM(p_args, struct db_hsi)->lightness = 500;
}

void ui_reset_adv_hsi(struct db_hsi *p_args)
{
    PARAM(p_args, struct db_hsi)->hsi.cct = 5600;
    PARAM(p_args, struct db_hsi)->hsi.hue = 100;
    PARAM(p_args, struct db_hsi)->hsi.sat = 50;
    PARAM(p_args, struct db_hsi)->lightness = 500;    
}

void ui_reset_source(struct db_source  *p_args)
{
    PARAM(p_args, struct db_source)->lightness = 500;
    PARAM(p_args, struct db_source)->source.type = 0;
    PARAM(p_args, struct db_source)->source.x = 0;
    PARAM(p_args, struct db_source)->source.y = 0;
}

void ui_reset_rgb(struct db_rgb *p_args)
{
    PARAM(p_args, struct db_rgb)->lightness = 500;
    PARAM(p_args, struct db_rgb)->rgb.r = 500;
    PARAM(p_args, struct db_rgb)->rgb.g = 500;
    PARAM(p_args, struct db_rgb)->rgb.b = 500;    
}

void ui_reset_gel(struct db_gel* p_args)
{
    PARAM(p_args, struct db_gel)->lightness = 500;
    PARAM(p_args, struct db_gel)->gel.type[0] = 0;
    PARAM(p_args, struct db_gel)->gel.type[1] = 0;
    PARAM(p_args, struct db_gel)->gel.brand= 0;    
}

void ui_reset_xy(struct db_xy * p_args)
{
    PARAM(p_args, struct db_xy )->lightness = 500;
    PARAM(p_args, struct db_xy )->xy.x = 3301;
    PARAM(p_args, struct db_xy )->xy.y = 3390;
}

void ui_reset_cct(struct db_cct* p_args)
{
    PARAM(p_args, struct db_cct)->lightness = 500;
    PARAM(p_args, struct db_cct)->cct.duv = 0;
    PARAM(p_args, struct db_cct)->cct.cct = 5600;
}

//void ui_reset_param_setting(uint8_t type, param_setting_t* p_setting)
//{
//    switch(type) {
//        case PARAM_SETTING_CCT:     ui_reset_cct(&p_setting->CCT);      break;
//        case PARAM_SETTING_HSI:     ui_reset_hsi(&p_setting->HSI);      break;
//        case PARAM_SETTING_RGB:     ui_reset_rgb(&p_setting->RGB);      break;
//        case PARAM_SETTING_GEL:     ui_reset_gel(&p_setting->GEL);      break;
//        case PARAM_SETTING_SOURCE:  ui_reset_source(&p_setting->SOURCE);break;
//        case PARAM_SETTING_XY:      ui_reset_xy(&p_setting->XY);        break;
//    }    
//}

void ui_reset_limit_setting(uint8_t type, struct db_fx_mode_limit_arg* p_setting) 
{
    switch (type) {
        case 0:
        {
            p_setting->cct.min_cct = UI_CCT_MIN;
            p_setting->cct.max_cct = 8000;
        }
        break;
        case 1:
        {
            p_setting->hsi.cct = 5600;
            p_setting->hsi.min_hue = 1;
            p_setting->hsi.max_hue = 360;
            p_setting->hsi.sat = 50;
        }
        break;
    }
}

void ui_reset_ble_status(void)
{
    ble_status_t *p_status = &g_ptUIStateData->ble_status;    
    p_status->status = BLE_RESET_IDLE;
    p_status->percent = 0;
    p_status->timecnt = 0;
}

void ui_reset_lumen_status(void)
{
    void *p_status = &g_ptUIStateData->lumen_status;
    memset(p_status, 0, sizeof(g_ptUIStateData->lumen_status));
}

void ui_set_lan_type(bool state)
{
    g_ptUIStateData->lan_state = state;
}

void ui_light_effect_index_save(uint8_t index)
{
    g_tUIAllData.sys_menu_model.light_effect_index = index;
}

uint8_t ui_accessory_pan_lock_get(uint8_t mode)
{
    if(mode == 0)
		return g_ptUIStateData->ele_accessory.ele_yoke_pan_lock;
	else
	{
		uint8_t acces_sel;
		
		data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &acces_sel);
		return g_ptUIStateData->ele_accessory.ele_yoke_pan_lock1[acces_sel];
	}
}

uint8_t ui_accessory_tilt_lock_get(uint8_t mode)
{
    if(mode == 0)
		return g_ptUIStateData->ele_accessory.ele_yoke_tilt_lock;
	else
	{
		uint8_t acces_sel;
		
		data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &acces_sel);
		return g_ptUIStateData->ele_accessory.ele_yoke_tilt_lock1[acces_sel];
	}
}

void ui_accessory_pan_lock_set(uint8_t mode, uint8_t value)
{
    if(mode == 0)
		g_ptUIStateData->ele_accessory.ele_yoke_pan_lock = value;
	else
	{
		uint8_t acces_sel;
		
		data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &acces_sel);
		g_ptUIStateData->ele_accessory.ele_yoke_pan_lock1[acces_sel] = value;
	}
}

void ui_accessory_tilt_lock_set(uint8_t mode, uint8_t value)
{
    if(mode == 0)
		g_ptUIStateData->ele_accessory.ele_yoke_tilt_lock = value;
	else
	{
		uint8_t acces_sel;
		
		data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &acces_sel);
		g_ptUIStateData->ele_accessory.ele_yoke_tilt_lock1[acces_sel] = value;
	}
}

uint8_t ui_light_effect_index_get(void)
{
    return g_tUIAllData.sys_menu_model.light_effect_index;
}


void ui_error_code_event_set(bool state)
{
	g_ptUIStateData->error_code_event = state;
}

bool ui_error_code_event_get(void)
{
	return g_ptUIStateData->error_code_event;
}

void ui_regognition_error_set(bool state)
{
	g_ptUIStateData->regognition_error = state;
}

bool ui_regognition_error_get(void)
{
	return g_ptUIStateData->regognition_error;
}

void ui_set_trigger_cfx_state(uint8_t state)
{
	s_ui_trigger_cfx = state;
}

uint8_t ui_get_trigger_cfx_state(void)
{
	return s_ui_trigger_cfx;
}

uint8_t ui_test_Fresnel_Industrial_mode_get(void)
{
	return g_ptUIStateData->test_data.Fresnel_Industrial ;
}

void ui_test_Fresnel_Industrial_mode_set( uint8_t angle )
{
	 g_ptUIStateData->test_data.Fresnel_Industrial = angle;
}

void ui_set_lumen_name(uint8_t *reset_flag, uint16_t time)
{
	char name[21] = {0};
	char ble_name[6] ={0};
	bool res;
	int8_t i = 0;
	if(time <= 1 && *reset_flag == 0)
	{
		*reset_flag = 1;
		dev_crmx_power_ctrl(1);
	}
	if(time >= 70 && *reset_flag == 1)
	{
		i = 0;
		*reset_flag = 2;
		#if PROJECT_TYPE==307
		memcpy(name, "ES_CS15-", 13);
		#endif
		#if PROJECT_TYPE==308
		memcpy(name, "ES_XT26-", 13);
		#endif
		data_center_read_config_data(SYS_CONFIG_BLE_SN, ble_name);
		i = 6;
		while(i >= 0)
		{
			if(ble_name[i] >= 'a' && ble_name[i] <= 'z')
			{
				ble_name[i] = ble_name[i] - 32;
			}
			i--;
		}
		memcpy(&name[13], ble_name, 6);
		name[19]='\0';
		res = lumenradio_set_device_name(&g_lumenradio_device, name);
		if(res != true)
		{
			lumenradio_set_device_name(&g_lumenradio_device, name);
		}
		res = lumenradio_set_work_mode(&g_lumenradio_device, LUMENRADIO_WORK_MODE_RECEIVER);
		if(res != true)
		{
			lumenradio_set_work_mode(&g_lumenradio_device, LUMENRADIO_WORK_MODE_RECEIVER);
		}
		res = lumenradio_unlink(&g_lumenradio_device);
		if(res == false)
		{
			lumenradio_unlink(&g_lumenradio_device);
		} 
		dev_crmx_power_ctrl(0);
		set_ble_power(1);
		set_ble_power_status(1);
		ui_ble_start_reset();
	}
}

void ui_set_ctr_spid_index(uint8_t index)
{
	ctr_last_spid_index = index;
}

uint8_t ui_get_ctr_spid_index(void)
{
	return ctr_last_spid_index;
}

void ui_restore_def_setting(void)
{
	uint8_t ble_uuid[6];
	data_center_read_config_data(SYS_CONFIG_BLE_SN, ble_uuid);

    void* p_args;
    (void)p_args;
#ifdef  PARAM
    #undef  PARAM
#endif
#define PARAM(param, type)      ((type*)param)    

#if 0
    g_tUIAllData.magic_number = UI_DATA_MAGIC_NUMBER;
    p_args = &g_tUIAllData.sys_menu_model;
    PARAM(p_args, sys_config_t)->frequency = 1100;
    PARAM(p_args, sys_config_t)->dmx_loss_behavior = DMX_LOSS_BEHAVIOR_HLS;
    PARAM(p_args, sys_config_t)->lang = LANG_TYPE_EN;
    PARAM(p_args, sys_config_t)->dmx_locked = false;
    PARAM(p_args, sys_config_t)->crmx_on = false;
    PARAM(p_args, sys_config_t)->output_mode = OUTPUT_MODE_MAX_OUTPUT;
    PARAM(p_args, sys_config_t)->work_mode =  WORK_MODE_SLAVE;
    PARAM(p_args, sys_config_t)->ble_on =  true;
    PARAM(p_args, sys_config_t)->dmx_addr =  1;
    PARAM(p_args, sys_config_t)->fan_mode = FAN_MODE_SMART;
    PARAM(p_args, sys_config_t)->curve_type = CURVE_TYPE_LINEAR;
    PARAM(p_args, sys_config_t)->dmx_profile_index = 0;
    PARAM(p_args, sys_config_t)->screensaver_index = 0;

    memset(PARAM(p_args, sys_config_t)->custom_fx_sel, 0, sizeof(PARAM(p_args, sys_config_t)->custom_fx_sel)/sizeof(uint8_t));
    int16_t* p_fx_int = (int16_t*)(PARAM(p_args, sys_config_t)->custom_fx_intensity);
    for (uint8_t i = 0; i < (sizeof(PARAM(p_args, sys_config_t)->custom_fx_intensity) / sizeof(int16_t)); i++)
    {
        p_fx_int[i] = 500;
    }
    
    PARAM(p_args, sys_config_t)->custom_fx_type = CUSTOM_FX_TYPE_PIKER;
    PARAM(p_args, sys_config_t)->studiomode_on = false;
    PARAM(p_args, sys_config_t)->light_effect_index = light_effect_get_index(PAGE_FIREWORKS);
    //光模式 [rgb]
    p_args = &g_tUIAllData.rgb_model;
    ui_reset_rgb(p_args);
    p_args = &g_tUIAllData.source_model;
    ui_reset_source(p_args);    
    p_args = &g_tUIAllData.gel_model;
    ui_reset_gel(p_args);
    p_args = &g_tUIAllData.xy_model;
    ui_reset_xy(p_args);    
    p_args = &g_tUIAllData.hsi_model;
    ui_reset_adv_hsi(p_args);
    p_args = &g_tUIAllData.cct_model;
    ui_reset_cct(p_args);
#endif
    data_center_factory_reset(LOCAL_ALL_DATA);
    data_center_read_config_data(SYS_CONFIG_ALL, &g_tUIAllData.sys_menu_model);

#if 0
    //光效 [狗仔队]
    p_args = &g_tUIAllData.paparazzi_model;
    fx_obj_init(&PARAM(p_args, page_paparazzi_t)->obj);
    p_args = &g_tUIAllData.paparazzi2_model;
    fx_obj_init(&PARAM(p_args, page_paparazzi_t)->obj);

    //光效 [派对]
    p_args = &g_tUIAllData.partylight_model;
    fx_obj_init(&PARAM(p_args, page_partylight_t)->obj);
    p_args = &g_tUIAllData.partylight2_model;
    fx_obj_init(&PARAM(p_args, page_partylight_t)->obj);

    //光效 [脉搏]
    p_args = &g_tUIAllData.pulsing_model;
    fx_obj_init(&PARAM(p_args, page_pulsing_t)->obj);
    for (uint8_t i = 0; i < PARAM_SETTING_MAX; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_pulsing_t)->setting[i]);
    }    

    p_args = &g_tUIAllData.pulsing2_model;
    fx_obj_init(&PARAM(p_args, page_pulsing_t)->obj);
    PARAM(p_args, page_pulsing_t)->param_index = 0;
    for (uint8_t i = 0; i < PARAM_SETTING_MAX; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_pulsing_t)->setting[i]);
    }    

    //光效 [闪光灯]
    p_args = &g_tUIAllData.strobe_model;
    fx_obj_init(&PARAM(p_args, page_strobe_t)->obj);
    PARAM(p_args, page_strobe_t)->param_index = 0;
    for (uint8_t i = 0; i < PARAM_SETTING_MAX; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_strobe_t)->setting[i]);
    }    

    p_args = &g_tUIAllData.strobe2_model;
    fx_obj_init(&PARAM(p_args, page_strobe_t)->obj);
    PARAM(p_args, page_strobe_t)->param_index = 0;
    for (uint8_t i = 0; i < PARAM_SETTING_MAX; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_strobe_t)->setting[i]);
    }    

    //光效 [电视]
    p_args = &g_tUIAllData.tv_model;
    fx_obj_init(&PARAM(p_args, page_tv_t)->obj);
    for (uint8_t i = 0; i < 2; i++) {
        ui_reset_limit_setting(i, &PARAM(p_args, page_tv_t)->setting[i]);
    }    

    p_args = &g_tUIAllData.tv2_model;
    fx_obj_init(&PARAM(p_args, page_tv_t)->obj);
    for (uint8_t i = 0; i < 2; i++) {
        ui_reset_limit_setting(i, &PARAM(p_args, page_tv_t)->setting[i]);
    }    
   
    //光效 [闪电]
    p_args =  &g_tUIAllData.lightning_model;
    fx_obj_init(&PARAM(p_args, page_lightning_t)->obj);
    PARAM(p_args, page_lightning_t)->param_index = 0;

    for (uint8_t i = 0; i < 2; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_lightning_t)->setting[i]);
    }    

    p_args =  &g_tUIAllData.lightning2_model;
    fx_obj_init(&PARAM(p_args, page_lightning_t)->obj);
    PARAM(p_args, page_lightning_t)->param_index = 0;

    for (uint8_t i = 0; i < 2; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_lightning_t)->setting[i]);
    }    

    //光效 [火焰]
    p_args =  &g_tUIAllData.fire_model;
    fx_obj_init(&PARAM(p_args, page_fire_t)->obj);
    PARAM(p_args, page_fire_t)->limit_index = 0;

    for (uint8_t i = 0; i < 2; i++) {
        ui_reset_limit_setting(i, &PARAM(p_args, page_fire_t)->setting[i]);
    }        

    p_args =  &g_tUIAllData.fire2_model;
    fx_obj_init(&PARAM(p_args, page_fire_t)->obj);
    PARAM(p_args, page_fire_t)->limit_index = 0;

    for (uint8_t i = 0; i < 2; i++) {
        ui_reset_limit_setting(i, &PARAM(p_args, page_fire_t)->setting[i]);
    }        

    //光效 [颜色追逐]
    p_args =  &g_tUIAllData.colorchase_model;
    fx_obj_init(&PARAM(p_args, page_colorchase_t)->obj);

    //光效 [警车]
    p_args = &g_tUIAllData.copcar_model;
    fx_obj_init(&PARAM(p_args, page_copcar_t)->obj);
    
    p_args = &g_tUIAllData.copcar2_model;
    fx_obj_init(&PARAM(p_args, page_copcar_t)->obj);

    //光效 [爆炸]
    p_args = &g_tUIAllData.explosion_model;
    fx_obj_init(&PARAM(p_args, page_explosion_t)->obj);
    PARAM(p_args, page_explosion_t)->param_index = 0;
    for (uint8_t i = 0; i < PARAM_SETTING_MAX; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_explosion_t)->setting[i]);
    }        

    p_args = &g_tUIAllData.explosion2_model;
    fx_obj_init(&PARAM(p_args, page_explosion_t)->obj);
    PARAM(p_args, page_explosion_t)->param_index = 0;
    for (uint8_t i = 0; i < PARAM_SETTING_MAX; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_explosion_t)->setting[i]);
    }        

    //光效 [俱乐部]
    p_args = &g_tUIAllData.clublights_model;
    fx_obj_init(&PARAM(p_args, page_clublights_t)->obj);
    
    //光效 [焊接]
    p_args = &g_tUIAllData.welding_model;
    fx_obj_init(&PARAM(p_args, page_welding_t)->obj);
    PARAM(p_args, page_welding_t)->param_index = 0;
    for (uint8_t i = 0; i < PARAM_SETTING_MAX; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_welding_t)->setting[i]);
    } 

    p_args = &g_tUIAllData.welding2_model;
    fx_obj_init(&PARAM(p_args, page_welding_t)->obj);
    PARAM(p_args, page_welding_t)->param_index = 0;
    for (uint8_t i = 0; i < PARAM_SETTING_MAX; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_welding_t)->setting[i]);
    }       

    //光效 [蜡烛]
    p_args = &g_tUIAllData.candle_model;
    fx_obj_init(&PARAM(p_args, page_candle_t)->obj);
    
    p_args = &g_tUIAllData.candle_model;
    fx_obj_init(&PARAM(p_args, page_candle_t)->obj);

    //光效 [烟火]
    p_args = &g_tUIAllData.fireworks_model;
    fx_obj_init(&PARAM(p_args, page_fireworks_t)->obj);

    //光效 [坏灯泡]
    p_args = &g_tUIAllData.faultybulb_model;
    fx_obj_init(&PARAM(p_args, page_faultbulb_t)->obj);
    PARAM(p_args, page_faultbulb_t)->param_index = 0;
    
    for (uint8_t i = 0; i < PARAM_SETTING_MAX; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_faultbulb_t)->setting[i]);
    }           

    p_args = &g_tUIAllData.faultybulb2_model;
    fx_obj_init(&PARAM(p_args, page_faultbulb_t)->obj);
    PARAM(p_args, page_faultbulb_t)->param_index = 0;
    
    for (uint8_t i = 0; i < PARAM_SETTING_MAX; i++) {
        ui_reset_param_setting(i, &PARAM(p_args, page_faultbulb_t)->setting[i]);
    }               
#endif    
#undef  PARAM  
#define PARAM(param, type)      (param)   
	data_center_write_config_data_no_event(SYS_CONFIG_BLE_SN, ble_uuid);
	g_ptUIStateData->firework_reset = 1;
	ui_set_eth_artnet_universe(0);
	ui_set_eth_sacn_universe(1);
	ui_set_eth_universe_state(0);
}

#ifndef UI_PLATFORM_MCU
bool crmx_paired = 0;
void crmx_is_paired_set( bool state )
{
     crmx_paired =  state ;
}
uint8_t crmx_is_paired(void)
{
    struct sys_info_crmx crmx_sta;
	data_center_read_sys_info(SYS_INFO_CRMX, &crmx_sta);
    return crmx_sta.pair_state;
}
#else
bool crmx_paired = 0;
void crmx_is_paired_set( bool state )
{
     crmx_paired =  state ;
}

uint8_t crmx_is_paired(void)
{
	struct sys_info_crmx crmx_sta;
	data_center_read_sys_info(SYS_INFO_CRMX, &crmx_sta);
    return crmx_sta.pair_state;
}

uint8_t crmx_get_link_streng(void)
{
	struct sys_info_crmx crmx_sta;
	data_center_read_sys_info(SYS_INFO_CRMX, &crmx_sta);
    return crmx_sta.crmx_link_streng;
}

bool crmx_is_not_dmx_data(void)
{
	struct sys_info_crmx crmx_sta;
	data_center_read_sys_info(SYS_INFO_CRMX, &crmx_sta);
    return crmx_sta.pair_state;
}

uint8_t gui_get_hs_mode(void)
{
	uint8_t hs_mode;
	
	data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &hs_mode);
	
	return hs_mode;
}

uint8_t gui_get_dmx_extension_number(uint8_t mode)
{
	uint8_t extension_index = 0;
	uint8_t num = 0;
	
	data_center_read_config_data(SYS_CONFIG_DMX_EXTENSION, &extension_index);
	switch(extension_index)
	{
		case 0:
			num = 0;
		break;
		case 1:
			num = mode == 0?3:6;
		break;
		case 2:
			num = 2;
		break;
		case 3:
			num = mode == 0?5:8;
		break;
		default:break;
	}
	
	return num;
}

uint8_t gui_get_dmx_profile_addr(void)
{
	uint8_t profile;
	uint8_t number = 0;
	struct db_dmx dmx_protocol;
	
	data_center_read_config_data(SYS_CONFIG_DMX_PROFILE, &profile);
	data_center_read_light_data(LIGHT_MODE_DMX, &dmx_protocol); 	
	#if PROJECT_TYPE==307
	switch(profile)
	{
		case 0:
			number = gui_get_dmx_extension_number(0);
			number += 8;
		break;
		case 1:
			number = gui_get_dmx_extension_number(0);
			number += 4;
		break;
		case 2:
			number = gui_get_dmx_extension_number(0);
			number += 8;
		break;
		case 3:
			number = gui_get_dmx_extension_number(0);
			number += 5;
		break;
		case 4:
			number = gui_get_dmx_extension_number(0);
			number += 5;
		break;
		case 5:
			number = gui_get_dmx_extension_number(0);
			number += 6;
		break;
		case 6:
			number = gui_get_dmx_extension_number(0);
			number += 4;
		break;
		case 7:
			number = gui_get_dmx_extension_number(0);
			number += 5;
		break;
		case 8:
			number = gui_get_dmx_extension_number(0);
			number += dmx_get_fx_extern_len(dmx_protocol.dmx_data.arg.fx.type, 0);
		break;
		case 9:
			number = gui_get_dmx_extension_number(0);
			number += 6;
		break;
		case 10:
			number = gui_get_dmx_extension_number(0);
			number += 11;
		break;
		case 11:
			number = gui_get_dmx_extension_number(1);
			number += 15;
		break;
		case 12:
			number = gui_get_dmx_extension_number(1);
			number += 7;
		break;
		case 13:
			number = gui_get_dmx_extension_number(1);
			number += 15;
		break;
		case 14:
			number = gui_get_dmx_extension_number(1);
			number += 9;
		break;
		case 15:
			number = gui_get_dmx_extension_number(1);
			number += 9;
		break;
		case 16:
			number = gui_get_dmx_extension_number(1);
			number += 7;
		break;
		case 17:
			number = gui_get_dmx_extension_number(1);
			number += 7;
		break;
		case 18:
			number = gui_get_dmx_extension_number(1);
			number += 6;
		break;
		case 19:
			number = gui_get_dmx_extension_number(1);
			number += dmx_get_fx_extern_len(dmx_protocol.dmx_data.arg.fx.type, 1);
		break;
		case 20:
			number = gui_get_dmx_extension_number(1);
			number += 11;
		break;
		case 21:
			number = gui_get_dmx_extension_number(1);
			number += 14;
		break;
	}
	#endif
	#if PROJECT_TYPE==308
	switch(profile)
	{
		case 0:
		case 1:
			number = gui_get_dmx_extension_number(0);
			number += 4;
		break;
		case 2:
			number = gui_get_dmx_extension_number(0);
			number += 7;
		break;
		case 3:
			number = gui_get_dmx_extension_number(1);
			number += 7;
		break;
		case 4:
			number = gui_get_dmx_extension_number(1);
			number += 4;
		break;
		case 5:
			number = gui_get_dmx_extension_number(1);
			number += 10;
		break;
	}
	#endif
	
	return number;
}

void ui_fresnel_get_angle_flag_set(uint8_t mode)
{
	s_ui_fresnel_angle_flag = mode;
}

uint8_t ui_fresnel_get_angle_flag_get(void)
{
	return s_ui_fresnel_angle_flag;
}

void ui_updata_start_flag_set(uint8_t mode)
{
	s_ui_updata_start_flag = mode;
}

uint8_t ui_updata_start_flag_get(void)
{
	return s_ui_updata_start_flag;
}

void ui_max_power_flag_set(uint8_t mode)
{
	s_ui_max_power_flag = mode;
}

uint8_t ui_max_power_flag_get(void)
{
	return s_ui_max_power_flag;
}

void ui_ctr_clear_time_flag_set(uint8_t state)
{
	g_ptUIStateData->test_data.clear_run_time_flag = state;
}

uint8_t ui_ctr_clear_time_flag_get(void)
{
	return g_ptUIStateData->test_data.clear_run_time_flag;
}

void ui_drv_clear_time_flag_set(uint8_t state)
{
	g_ptUIStateData->test_data.clear_davice_run_time_flag = state;
}

uint8_t ui_drv_clear_time_flag_get(void)
{
	return g_ptUIStateData->test_data.clear_davice_run_time_flag;
}

float ui_accessory_get_angle_limit(float angle, uint8_t mode)  //mode 0--水平   1--俯仰
{
	struct sys_info_accessories access_info;
	uint8_t tiltm;
	float end_angle = 0.0;
	uint8_t no_access_sel = 0;
	uint8_t no_tiltm[12];
	int16_t ele_yoke_tilt1[12][2];  
	int16_t ele_yoke_pan1[12][2];   
	int16_t ele_yoke_tilt[2];  
	int16_t ele_yoke_pan[2];  
	
	data_center_read_config_data(SYS_CONFIG_ANGLE_LIMIT, &tiltm);
	data_center_read_sys_info(SYS_INFO_ACCESS, &access_info);
	
	if(access_info.fresnel_state == 1 || access_info.optical_20_access_state == 1 || access_info.optical_30_access_state == 1 ||access_info.optical_50_access_state == 1)
	{
		if(tiltm == 1)
		{
			if(mode == 1)
			{
				data_center_read_config_data(SYS_CONFIG_YOKE_TILT_LIMIT, ele_yoke_tilt);
				if(access_info.fresnel_state == 1 || access_info.optical_20_access_state == 1)
				{
					if(ele_yoke_tilt[0] <= -120)
						ele_yoke_tilt[0] = -120;
					if(ele_yoke_tilt[0] >= 120)
						ele_yoke_tilt[0] = 120;
					if(ele_yoke_tilt[1] >= 120)
						ele_yoke_tilt[1] = 120;
					if(ele_yoke_tilt[1] <= -120)
						ele_yoke_tilt[1] = -120;
					if(angle <= ele_yoke_tilt[0])
						angle = ele_yoke_tilt[0];
					if(angle >= ele_yoke_tilt[1])
						angle = ele_yoke_tilt[1];
				}
				else
				{
					if(angle <= ANGLE_LIMIT_DEFAULT_MIN)
						angle = ANGLE_LIMIT_DEFAULT_MIN;
					if(angle >= ANGLE_LIMIT_DEFAULT_MAX)
						angle = ANGLE_LIMIT_DEFAULT_MAX;
				}
			}
			else
			{
				data_center_read_config_data(SYS_CONFIG_YOKE_PAN_LIMIT, ele_yoke_pan);
				if(ele_yoke_pan[0] <= ANGLE_LIMIT_MIN)
					ele_yoke_pan[0] = ANGLE_LIMIT_MIN;
				if(ele_yoke_pan[0] >= ANGLE_LIMIT_MAX)
					ele_yoke_pan[0] = ANGLE_LIMIT_MAX;
				if(ele_yoke_pan[1] >= ANGLE_LIMIT_MAX)
					ele_yoke_pan[1] = ANGLE_LIMIT_MAX;
				if(ele_yoke_pan[1] <= ANGLE_LIMIT_MIN)
					ele_yoke_pan[1] = ANGLE_LIMIT_MIN;
				if(angle <= ele_yoke_pan[0])
					angle = ele_yoke_pan[0];
				if(angle >= ele_yoke_pan[1])
					angle = ele_yoke_pan[1];
			}
		}
		else
		{
			if(mode == 1)
			{
				if(angle <= ANGLE_LIMIT_DEFAULT_MIN)
					angle = ANGLE_LIMIT_DEFAULT_MIN;
				if(angle >= ANGLE_LIMIT_DEFAULT_MAX)
					angle = ANGLE_LIMIT_DEFAULT_MAX;
			}
			else
			{
				if(angle <= ANGLE_LIMIT_MIN)
					angle = ANGLE_LIMIT_MIN;
				if(angle >= ANGLE_LIMIT_MAX)
					angle = ANGLE_LIMIT_MAX;
			}
		}
	}
	else
	{
		data_center_read_config_data(SYS_CONFIG_NO_ANGLE_LIMIT, no_tiltm);
		data_center_read_config_data(SYS_CONFIG_NOT_ACCESS_SELECT, &no_access_sel);
		data_center_read_config_data(SYS_CONFIG_YOKE_NO_TILT_LIMIT, ele_yoke_tilt1);
		data_center_read_config_data(SYS_CONFIG_YOKE_NO_PAN_LIMIT, ele_yoke_pan1);
		if(no_tiltm[no_access_sel] == 1)
		{
			if(mode == 1)
			{
				if(no_access_sel <= 6)
				{
					if(ele_yoke_tilt1[no_access_sel][0] <= aputure_limit_angle_value[no_access_sel].lower_limit_angle)
						ele_yoke_tilt1[no_access_sel][0] = aputure_limit_angle_value[no_access_sel].lower_limit_angle;
					if(ele_yoke_tilt1[no_access_sel][0] >= aputure_limit_angle_value[no_access_sel].upper_limit_angle)
						ele_yoke_tilt1[no_access_sel][0] = aputure_limit_angle_value[no_access_sel].upper_limit_angle;
					if(ele_yoke_tilt1[no_access_sel][1] >= aputure_limit_angle_value[no_access_sel].upper_limit_angle)
						ele_yoke_tilt1[no_access_sel][1] = aputure_limit_angle_value[no_access_sel].upper_limit_angle;
					if(ele_yoke_tilt1[no_access_sel][1] <= aputure_limit_angle_value[no_access_sel].lower_limit_angle)
						ele_yoke_tilt1[no_access_sel][1] = aputure_limit_angle_value[no_access_sel].lower_limit_angle;
				}
				else
				{
					if(ele_yoke_tilt1[no_access_sel][0] <= ANGLE_LIMIT_DEFAULT_MIN)
						ele_yoke_tilt1[no_access_sel][0] = ANGLE_LIMIT_DEFAULT_MIN;
					if(ele_yoke_tilt1[no_access_sel][0] >= ANGLE_LIMIT_DEFAULT_MAX)
						ele_yoke_tilt1[no_access_sel][0] = ANGLE_LIMIT_DEFAULT_MAX;
					if(ele_yoke_tilt1[no_access_sel][1] >= ANGLE_LIMIT_DEFAULT_MAX)
						ele_yoke_tilt1[no_access_sel][1] = ANGLE_LIMIT_DEFAULT_MAX;
					if(ele_yoke_tilt1[no_access_sel][1] <= ANGLE_LIMIT_DEFAULT_MIN)
						ele_yoke_tilt1[no_access_sel][1] = ANGLE_LIMIT_DEFAULT_MIN;
				}
				if(angle <= ele_yoke_tilt1[no_access_sel][0])
					angle = ele_yoke_tilt1[no_access_sel][0];
				if(angle >= ele_yoke_tilt1[no_access_sel][1])
					angle = ele_yoke_tilt1[no_access_sel][1];
				
			}
			else
			{
				if(ele_yoke_pan1[no_access_sel][0] <= ANGLE_LIMIT_MIN)
					ele_yoke_pan1[no_access_sel][0] = ANGLE_LIMIT_MIN;
				if(ele_yoke_pan1[no_access_sel][1] >= ANGLE_LIMIT_MAX)
					ele_yoke_pan1[no_access_sel][1] = ANGLE_LIMIT_MAX;
				if(angle <= ele_yoke_pan1[no_access_sel][0])
					angle = ele_yoke_pan1[no_access_sel][0];
				if(angle >= ele_yoke_pan1[no_access_sel][1])
					angle = ele_yoke_pan1[no_access_sel][1];
			}
		}
		else
		{
			if(mode == 1)
			{
				if(angle <= ANGLE_LIMIT_DEFAULT_MIN)
					angle = ANGLE_LIMIT_DEFAULT_MIN;
				if(angle >= ANGLE_LIMIT_DEFAULT_MAX)
					angle = ANGLE_LIMIT_DEFAULT_MAX;
			}
			else
			{
				if(angle <= ANGLE_LIMIT_MIN)
					angle = ANGLE_LIMIT_MIN;
				if(angle >= ANGLE_LIMIT_MAX)
					angle = ANGLE_LIMIT_MAX;
			}
		}
	}
	
	end_angle = angle;
	
	return end_angle;
}	


void ui_motor_reset_status_set(uint8_t state)
{
	s_ui_set_motor_reset_flag = state;
}

uint8_t ui_motor_reset_status_get(void)
{
	return s_ui_set_motor_reset_flag;
}

uint8_t ui_get_work_update_number_flag(void)
{
	return s_work_get_update_flag;
}

void ui_set_work_update_number_flag(uint8_t state)
{
	s_work_get_update_flag = state;
}

uint8_t ui_get_batch_update_serial_num(void)
{
	return s_batch_update_serial_num;
}

void ui_set_batch_update_serial_num(uint8_t num)
{
	s_batch_update_serial_num = num;
}

uint8_t ui_get_work_update_number(void)
{
	struct sys_info_update_name  update_name;
	
	data_center_read_sys_info(SYS_INFO_WORK_UPDATE, &update_name);	
	
	return update_name.file_number;
}

void ui_set_upgrade_timeout_clear(void)
{
	s_ui_upgrade_timeout = 0;
}

void ui_set_upgrade_timeout_inc(void)
{
	s_ui_upgrade_timeout++;
}

uint16_t ui_set_upgrade_timeout_get(void)
{
	return s_ui_upgrade_timeout;
}

void ui_set_ele_reset_start_flag(uint8_t state)
{
	s_ele_reset_flag = state;
}

uint16_t ui_get_ele_reset_start_flag(void)
{
	return s_ele_reset_flag;
}

#endif // DEBUG


static bool preset_task_light_mode_valid(enum light_mode mode)
{
    switch (mode)
    {
        case LIGHT_MODE_CCT:
        case LIGHT_MODE_HSI:
        case LIGHT_MODE_RGB:
        case LIGHT_MODE_GEL:
        case LIGHT_MODE_XY:
        case LIGHT_MODE_SOURCE:
        case LIGHT_MODE_FX_PULSING:
        case LIGHT_MODE_FX_PULSING_II:
		case LIGHT_MODE_FX_STROBE:
        case LIGHT_MODE_FX_STROBE_II:
        case LIGHT_MODE_FX_EXPLOSION:
        case LIGHT_MODE_FX_EXPLOSION_II:
        case LIGHT_MODE_FX_FAULT_BULB:
        case LIGHT_MODE_FX_FAULT_BULB_II:
        case LIGHT_MODE_FX_WELDING_II:
        case LIGHT_MODE_FX_COP_CAR_II:
        case LIGHT_MODE_FX_CANDLE:
        case LIGHT_MODE_FX_CLUBLIGHTS:
        case LIGHT_MODE_FX_COLOR_CHASE:
        case LIGHT_MODE_FX_TV:
        case LIGHT_MODE_FX_TV_II:
        case LIGHT_MODE_FX_PAPARAZZI:
        case LIGHT_MODE_FX_LIGHTNING:
        case LIGHT_MODE_FX_LIGHTNING_II:
        case LIGHT_MODE_FX_FIREWORKS:
        case LIGHT_MODE_FX_FIREWORKS_II:
        case LIGHT_MODE_FX_FIRE:
        case LIGHT_MODE_FX_FIRE_II:
        case LIGHT_MODE_FX_PARTY_LIGHTS:
            return true;
        default: break;
    }
    return false;
}

