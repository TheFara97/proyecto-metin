# coding: latin_1

import ui, net, chr, localeInfo

IMG_DIR = "d:/ymir work/ui/game/autoskill/"

skillData = {
    0: {
        "image":IMG_DIR+"warrior.tga",
        "name":localeInfo.TOOLTIP_WARRIOR,
        "skillIndex1":[localeInfo.SKILL_GROUP_WARRIOR_1,localeInfo.AUTOSKILL_0_1],
        "skillIndex2":[localeInfo.SKILL_GROUP_WARRIOR_2,localeInfo.AUTOSKILL_0_2],
    },
    1: {
        "image":IMG_DIR+"assassin.tga",
        "name":localeInfo.TOOLTIP_ASSASSIN,
        "skillIndex1":[localeInfo.SKILL_GROUP_ASSASSIN_1,localeInfo.AUTOSKILL_1_1],
        "skillIndex2":[localeInfo.SKILL_GROUP_ASSASSIN_2,localeInfo.AUTOSKILL_1_2],
    },
    2: {
        "image":IMG_DIR+"sura.tga",
        "name":localeInfo.TOOLTIP_SURA,
        "skillIndex1":[localeInfo.SKILL_GROUP_SURA_1,localeInfo.AUTOSKILL_2_1],
        "skillIndex2":[localeInfo.SKILL_GROUP_SURA_2,localeInfo.AUTOSKILL_2_2],
    },
    3: {
        "image":IMG_DIR+"shaman.tga",
        "name":localeInfo.TOOLTIP_SHAMAN,
        "skillIndex1":[localeInfo.SKILL_GROUP_SHAMAN_1,localeInfo.AUTOSKILL_3_1],
        "skillIndex2":[localeInfo.SKILL_GROUP_SHAMAN_2,localeInfo.AUTOSKILL_3_2],
    },
}


class AutoSkillSelect(ui.ImageBox):
    def __del__(self):
        ui.ImageBox.__del__(self)
    def __init__(self):
        ui.ImageBox.__init__(self)
        self.Destroy()
        self.LoadWindow()
    def Destroy(self):
        self.children = {}

    def LoadWindow(self):
        if net.GetMainActorSkillGroup() != 0:
            return
        playerRace = net.GetMainActorRace()
        playerJob = chr.RaceToJob(playerRace)
        if not skillData.has_key(playerJob):
            return

        playerSkillData = skillData[playerJob]

        self.AddFlag("attach")
        self.AddFlag("movable")
        self.AddFlag("float")
        self.LoadImage(playerSkillData["image"])
        self.SetCenterPosition()

        skillName1 = ui.TextLine()
        skillName1.SetParent(self)
        skillName1.SetPosition(207,46)
        skillName1.SetFontName("Tahoma:13")
        skillName1.SetPackedFontColor(0xffFFC800)
        skillName1.SetHorizontalAlignCenter()
        skillName1.SetOutline()
        skillName1.SetText(playerSkillData["skillIndex1"][0]+" "+playerSkillData["name"])
        skillName1.Show()
        self.children["skillName1"] = skillName1

        skillDesc1 = ui.TextLine()
        skillDesc1.SetParent(self)
        skillDesc1.SetPosition(207,59)
        skillDesc1.SetFontName("Tahoma:13")
        skillDesc1.SetHorizontalAlignCenter()
        skillDesc1.SetOutline()
        skillDesc1.SetText(playerSkillData["skillIndex1"][1])
        skillDesc1.Show()
        self.children["skillDesc1"] = skillDesc1

        skillBtn1 = ui.Button()
        skillBtn1.SetParent(self)
        skillBtn1.SetUpVisual(IMG_DIR+"accept_btn_0.tga")
        skillBtn1.SetOverVisual(IMG_DIR+"accept_btn_1.tga")
        skillBtn1.SetDownVisual(IMG_DIR+"accept_btn_2.tga")
        skillBtn1.SetPosition(176,80)
        skillBtn1.SetEvent(ui.__mem_func__(self.ClickSkillButton),1)
        skillBtn1.Show()
        self.children["skillBtn1"] = skillBtn1

        skillName2 = ui.TextLine()
        skillName2.SetParent(self)
        skillName2.SetPosition(207,138)
        skillName2.SetFontName("Tahoma:13")
        skillName2.SetPackedFontColor(0xffFFC800)
        skillName2.SetHorizontalAlignCenter()
        skillName2.SetOutline()
        skillName2.SetText(playerSkillData["skillIndex2"][0]+" "+playerSkillData["name"])
        skillName2.Show()
        self.children["skillName2"] = skillName2

        skillDesc2 = ui.TextLine()
        skillDesc2.SetParent(self)
        skillDesc2.SetPosition(207,153)
        skillDesc2.SetFontName("Tahoma:13")
        skillDesc2.SetHorizontalAlignCenter()
        skillDesc2.SetOutline()
        skillDesc2.SetText(playerSkillData["skillIndex2"][1])
        skillDesc2.Show()
        self.children["skillDesc2"] = skillDesc2

        skillBtn2 = ui.Button()
        skillBtn2.SetParent(self)
        skillBtn2.SetUpVisual(IMG_DIR+"accept_btn_0.tga")
        skillBtn2.SetOverVisual(IMG_DIR+"accept_btn_1.tga")
        skillBtn2.SetDownVisual(IMG_DIR+"accept_btn_2.tga")
        skillBtn2.SetPosition(176,173)
        skillBtn2.SetEvent(ui.__mem_func__(self.ClickSkillButton),2)
        skillBtn2.Show()
        self.children["skillBtn2"] = skillBtn2

    def ClickSkillButton(self, index):
        net.SendChatPacket("/skillauto select %d"%(int(index)))
        self.Hide()

    def OnPressEscapeKey(self):
        self.Hide()
        return True
