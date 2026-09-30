import os
import ui
import player
import mouseModule
import net
import app
import snd
import item
import player
import chat
import grp
import uiScriptLocale
import localeInfo
import constInfo
import ime
import wndMgr
import playerSettingModule

FACE_IMAGE_DICT = {
	playerSettingModule.RACE_WARRIOR_M	: "icon/face/warrior_m.tga",
	playerSettingModule.RACE_WARRIOR_W	: "icon/face/warrior_w.tga",
	playerSettingModule.RACE_ASSASSIN_M	: "icon/face/assassin_m.tga",
	playerSettingModule.RACE_ASSASSIN_W	: "icon/face/assassin_w.tga",
	playerSettingModule.RACE_SURA_M		: "icon/face/sura_m.tga",
	playerSettingModule.RACE_SURA_W		: "icon/face/sura_w.tga",
	playerSettingModule.RACE_SHAMAN_M	: "icon/face/shaman_m.tga",
	playerSettingModule.RACE_SHAMAN_W	: "icon/face/shaman_w.tga",
}
if app.ENABLE_WOLFMAN_CHARACTER:
	FACE_IMAGE_DICT.update({playerSettingModule.RACE_WOLFMAN_M  : "icon/face/wolfman_m.tga",})


class IsimDegistirIzi(ui.ScriptWindow):
	POSITIVE_COLOR = grp.GenerateColor(0.5411, 0.7254, 0.5568, 1.0)
	NEGATIVE_COLOR = grp.GenerateColor(0.9, 0.4745, 0.4627, 1.0)
	
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()
	
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/isimiziwindow.py")
		except:
			import exception
			exception.Abort("IsimIziWindow.LoadWindow.LoadObject")
			
		try:
			self.board = self.GetChild("board")
			self.boardtitle = self.GetChild("IsimIzi_TitleBar")
			
			self.isimimg = self.GetChild("IsimSlot")
			self.isimgir = self.GetChild("isim_gir")
			self.IsimButton = self.GetChild("IsimButton")

			self.faceImage = self.GetChild("Face_Image")
			
			
			#Event
			self.boardtitle.SetCloseEvent(ui.__mem_func__(self.Close))
			self.IsimButton.SetEvent(ui.__mem_func__(self.isimizigir,))
			
		except:
			import exception
			exception.Abort("IsimIziWindow.LoadWindow.BindObject")

		## FaceImage
		race = net.GetMainActorRace()
		try:
			faceImageName = FACE_IMAGE_DICT[race]

			try:
				self.faceImage.LoadImage(faceImageName)
			except:
				print "CharacterWindow.RefreshCharacter(race=%d, faceImageName=%s)" % (race, faceImageName)
				self.faceImage.Hide()

		except KeyError:
			self.faceImage.Hide()
			
	def isimizigir(self):
		if self.isimgir.GetText() == "" or len(self.isimgir.GetText()) < 3:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Yeni Ýsmin En Az 3 Karakter Olmalýdýr")
			return
		elif self.isimgir.GetText() == "" or len(self.isimgir.GetText()) > 12:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Yeni Ýsmin En Fazlaa 12 Karakter Olmalýdýr")
			return

		player.SetAutoName(self.isimgir.GetText())
		self.Close()
		return True