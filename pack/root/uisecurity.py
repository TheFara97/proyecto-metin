import ui
import localeInfo
import player
import app
import chat
import uiCommon
import wndMgr


class BaseCaptcha(ui.Board):
    """Base class for all CAPTCHA implementations"""
    
    # Common constants
    REMAINING_TIME = 20
    MAX_WRONG_ATTEMPTS = 2
    
    def __init__(self):
        super(BaseCaptcha, self).__init__()
        self._initialize_common_attributes()
        self.Constructor()
        self.LoadWindow()
    
    def __del__(self):
        super(BaseCaptcha, self).__del__()
    
    def _initialize_common_attributes(self):
        """Initialize common attributes for all CAPTCHA types"""
        self.endTime = 0
        self.missAnswerCount = 0
        self.BackgroundShadow = None
        self.remainingTimeGauge = None
        self.remainingTimeText = None
    
    def Constructor(self):
        """Constructor method to be overridden by subclasses - maintains original API"""
        self._setup_background_shadow()
    
    def _setup_background_shadow(self):
        """Setup common background shadow"""
        self.BackgroundShadow = uiCommon.BackgroundShadow(1)
        self.BackgroundShadow.Hide()
        self.SetParent(self.BackgroundShadow)
    
    def Open(self):
        """Open the CAPTCHA window - maintains original API"""
        self.BackgroundShadow.Show()
        self.Show()
        self._reset_state()
        self._generate_challenge()
        self.endTime = app.GetTime() + self.REMAINING_TIME
    
    def Close(self):
        """Close the CAPTCHA window - maintains original API"""
        self.Hide()
        self.BackgroundShadow.Hide()
    
    def _reset_state(self):
        """Reset CAPTCHA state - to be overridden by subclasses"""
        self.missAnswerCount = 0
    
    def _generate_challenge(self):
        """Generate new challenge - to be overridden by subclasses"""
        pass
    
    def LoadWindow(self):
        """Load window UI - to be overridden by subclasses"""
        pass
    
    def _setup_common_ui_elements(self):
        """Setup common UI elements like timer"""
        self.remainingTimeGauge = ui.Gauge()
        self.remainingTimeGauge.SetParent(self)
        self.remainingTimeGauge.MakeGauge(190, "white")
        self.remainingTimeGauge.Show()
        
        self.remainingTimeText = ui.TextLine()
        self.remainingTimeText.SetParent(self)
        self.remainingTimeText.Show()
    
    def TimeIsUp(self):
        """Handle timeout - maintains original API"""
        player.BotControlFail()
        self.Close()
    
    def CorrectAnswer(self):
        """Handle correct answer - maintains original API"""
        chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SECURITY_ANSWER_SUCCESS)
        player.SetBotControlState(False)
        player.SetBotControlWindow()
        self.Close()
    
    def WrongAnswer(self):
        """Handle wrong answer - maintains original API"""
        if self.missAnswerCount >= self.MAX_WRONG_ATTEMPTS:
            player.BotControlFail()
            self.Close()
        else:
            self.missAnswerCount += 1
            chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SECURITY_YOU_HAVE_ONE_MORE_CHANGE)
            self._handle_wrong_answer()
    
    def _handle_wrong_answer(self):
        """Handle wrong answer logic - to be overridden by subclasses"""
        self.endTime -= 5
        self._generate_challenge()
    
    def OnUpdate(self):
        """Update method - maintains original API"""
        if app.GetTime() >= self.endTime:
            self.TimeIsUp()
        
        self._update_timer_display()
    
    def _update_timer_display(self):
        """Update timer display"""
        remaining_time = max(0, self.endTime - app.GetTime())
        if self.remainingTimeText:
            self.remainingTimeText.SetText(localeInfo.SECURITY_REMAINING_TIME.format(remaining_time))
        if self.remainingTimeGauge:
            self.remainingTimeGauge.SetPercentage(remaining_time, self.REMAINING_TIME)


class WritingCaptcha(BaseCaptcha):
    """Writing CAPTCHA where user must type numbers from images"""
    
    PATH_IMAGE = "zaris/securityvalues/"
    PATH_BUTTON = "offlineshop/lightwork/searchshop/"
    
    def Constructor(self):
        """Constructor method - maintains original API"""
        super(WritingCaptcha, self).Constructor()
        self._initialize_writing_attributes()
    
    def _initialize_writing_attributes(self):
        """Initialize attributes specific to writing CAPTCHA"""
        self.textList = []
        self.answerList = [0, 0, 0, 0, 0]
        self.buttonList = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.correctAnswer = None
        self.answerText = None
        self.editLingBG = None
        self.boardBackground = None
        self.textBG = None
        self.questionText = None
        self.clearButton = None
        self.confirmButton = None
    
    def _generate_challenge(self):
        """Generate new writing challenge"""
        for newAnswer in xrange(5):
            self.answerList[newAnswer] = self.GetRandomNum(True)
            if len(self.textList) > newAnswer and self.textList[newAnswer]:
                image_path = self.PATH_IMAGE + "{}/".format(self.answerList[newAnswer]) + "{}.dds".format(self.GetRandomNum())
                self.textList[newAnswer].LoadImage(image_path)
    
    def LoadWindow(self):
        """Load window UI - maintains original API"""
        self.SetSize(240, 290)
        self.SetCenterPosition()
        
        self._create_input_field()
        self._create_board_background()
        self._create_image_displays()
        self._create_number_buttons()
        self._create_control_buttons()
        self._setup_common_ui_elements()
        self._position_timer_elements()
    
    def _create_input_field(self):
        """Create input field for answer"""
        self.editLingBG = ui.ExpandedImageBox()
        self.editLingBG.SetParent(self)
        self.editLingBG.SetPosition(55, 105)
        self.editLingBG.SetSize(200, 30)
        self.editLingBG.LoadImage("d:/ymir work/ui/public/Parameter_Slot_04.sub")
        self.editLingBG.Show()
        
        self.answerText = ui.TextLine()
        self.answerText.SetParent(self.editLingBG)
        self.answerText.SetPosition(45, 2)
        self.answerText.SetText("")
        self.answerText.Show()
        
        self.correctAnswer = ui.TextLine()
        self.correctAnswer.SetText("")
    
    def _create_board_background(self):
        """Create board background"""
        self.boardBackground = ui.ThinBoard()
        self.boardBackground.SetParent(self)
        self.boardBackground.SetPosition(10, 10)
        self.boardBackground.SetSize(220, 270)
        self.boardBackground.Show()
        
        self.textBG = ui.ThinBoard()
        self.textBG.SetParent(self.boardBackground)
        self.textBG.SetPosition(5, 5)
        self.textBG.SetSize(210, 64)
        self.textBG.Show()
    
    def _create_image_displays(self):
        """Create image display elements"""
        for num in xrange(6):
            self.textList.append(num)
            self.textList[num] = ui.ExpandedImageBox()
            self.textList[num].SetParent(self.textBG)
            self.textList[num].SetPosition(16 + (num * 36), 0)
            self.textList[num].Show()
        
        self.questionText = ui.MakeTextLineNew(self.textBG, 40, 70, localeInfo.SECURITY_SELECT_ANSWER)
    
    def _create_number_buttons(self):
        """Create number input buttons"""
        button_positions = [
            (1, 3, 185), (4, 6, 161), (7, 9, 137)  # (start, end, y_pos)
        ]
        
        for start, end, y_pos in button_positions:
            for btn in xrange(start, end + 1):
                x_pos = -45 + ((btn - start + 1) * 67)
                self.buttonList[btn] = ui.MakeButton(
                    self, x_pos, y_pos, False, self.PATH_IMAGE,
                    "small_button_0.png", "small_button_1.png", "small_button_2.png"
                )
                self.buttonList[btn].SetText("{}".format(btn))
                self.buttonList[btn].SetEvent(ui.__mem_func__(self.PressNumber), btn)
    
    def _create_control_buttons(self):
        """Create control buttons (clear, 0, confirm)"""
        self.clearButton = ui.MakeButton(
            self, -45 + 67, 209, False, self.PATH_IMAGE,
            "small_button_0.png", "small_button_1.png", "small_button_2.png"
        )
        self.clearButton.SetText(localeInfo.SECURITY_CLEAR)
        self.clearButton.SetEvent(ui.__mem_func__(self.ClearAnswerText))
        
        self.buttonList[0] = ui.MakeButton(
            self, -45 + 2 * 67, 209, False, self.PATH_IMAGE,
            "small_button_0.png", "small_button_1.png", "small_button_2.png"
        )
        self.buttonList[0].SetText("0")
        self.buttonList[0].SetEvent(ui.__mem_func__(self.PressNumber), 0)
        
        self.confirmButton = ui.MakeButton(
            self, -45 + 3 * 67, 209, False, self.PATH_IMAGE,
            "small_button_0.png", "small_button_1.png", "small_button_2.png"
        )
        self.confirmButton.SetText(localeInfo.SECURITY_CONFIRM)
        self.confirmButton.SetEvent(ui.__mem_func__(self.ConfirmAnswer))
    
    def _position_timer_elements(self):
        """Position timer elements"""
        if self.remainingTimeGauge:
            self.remainingTimeGauge.SetPosition(22, 240)
        if self.remainingTimeText:
            self.remainingTimeText.SetPosition(75, 250)
    
    def GetRandomNum(self, folderList=False):
        """Get random number - maintains original API"""
        if folderList:
            return app.GetRandom(0, 9)
        return app.GetRandom(0, 7)
    
    def PressNumber(self, num):
        """Handle number button press - maintains original API"""
        if len(self.answerText.GetText()) >= 5:
            return
        self.answerText.SetText(self.answerText.GetText() + str(num))
    
    def ClearAnswerText(self):
        """Clear answer text - maintains original API"""
        self.answerText.SetText("")
    
    def ConfirmAnswer(self):
        """Confirm answer - maintains original API"""
        self._build_correct_answer()
        
        if self.correctAnswer.GetText() == self.answerText.GetText():
            self.CorrectAnswer()
        else:
            self.WrongAnswer()
    
    def _build_correct_answer(self):
        """Build correct answer string"""
        self.correctAnswer.SetText("")
        for ans_letter in self.answerList:
            self.correctAnswer.SetText(self.correctAnswer.GetText() + str(ans_letter))
    
    def _handle_wrong_answer(self):
        """Handle wrong answer for writing CAPTCHA"""
        super(WritingCaptcha, self)._handle_wrong_answer()
        self.answerText.SetText("")


class FindTheDifferentCaptcha(BaseCaptcha):
    """Find the different image CAPTCHA"""
    
    PATH_IMAGE = "zaris/pickthewrongone/"
    PATH_BUTTON = "offlineshop/lightwork/searchshop/"
    
    def Constructor(self):
        """Constructor method - maintains original API"""
        super(FindTheDifferentCaptcha, self).Constructor()
        self._initialize_find_different_attributes()
    
    def _initialize_find_different_attributes(self):
        """Initialize attributes specific to find different CAPTCHA"""
        self.imageList = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        self.selectedIcon = None
        self.contractImage = None
        self.randomImage = None
        self.randomPosX = []
        self.randomPosY = [50, 108, 166]
        self.bgImage = None
        self.questionText = None
    
    def _generate_challenge(self):
        """Generate new find different challenge"""
        self.selectedIcon = self.GetRandomNum(True)
        self.contractImage = self.GetRandomNum()
        self.randomPosX = []
        
        self._create_image_grid()
        self._create_different_image()
    
    def _create_image_grid(self):
        """Create grid of similar images"""
        for image in xrange(12):
            x_pos, y_pos = self._calculate_image_position(image)
            
            self.imageList[image] = ui.MakeButton(
                self, x_pos, y_pos, False, self.PATH_IMAGE,
                "{}/{}.png".format(self.selectedIcon, self.contractImage),
                "", "", True
            )
            self.imageList[image].SetEvent(ui.__mem_func__(self.WrongImage))
            self.randomPosX.append(x_pos)
    
    def _calculate_image_position(self, image_index):
        """Calculate position for image at given index"""
        if image_index < 4:
            return (30 + 58 * image_index, 50)
        elif image_index < 8:
            return (30 + 58 * (image_index - 4), 108)
        else:
            return (30 + 58 * (image_index - 8), 166)
    
    def _create_different_image(self):
        """Create the different image that user should find"""
        different_image = 0 if self.contractImage == 1 else 1
        random_x = self.randomPosX[app.GetRandom(0, 11)]
        random_y = self.randomPosY[app.GetRandom(0, 2)]
        
        self.randomImage = ui.MakeButton(
            self, random_x, random_y, False, self.PATH_IMAGE,
            "{}/{}.png".format(self.selectedIcon, different_image),
            "", "", True
        )
        self.randomImage.SetEvent(ui.__mem_func__(self.CorrectImage))
    
    def LoadWindow(self):
        """Load window UI - maintains original API"""
        self.SetSize(280, 275)
        self.SetCenterPosition()
        
        self._create_background()
        self._create_question_text()
        self._setup_common_ui_elements()
        self._position_timer_elements()
    
    def _create_background(self):
        """Create background elements"""
        self.bgImage = ui.ThinBoard()
        self.bgImage.SetParent(self)
        self.bgImage.SetSize(260, 255)
        self.bgImage.SetPosition(10, 10)
        self.bgImage.Show()
    
    def _create_question_text(self):
        """Create question text"""
        self.questionText = ui.TextLine()
        self.questionText.SetParent(self)
        self.questionText.SetPosition(40, 20)
        self.questionText.SetText(localeInfo.SECURITY_FIND_THE_DIFFERENT)
        self.questionText.Show()
    
    def _position_timer_elements(self):
        """Position timer elements"""
        if self.remainingTimeGauge:
            self.remainingTimeGauge.SetPosition(42, 235)
        if self.remainingTimeText:
            self.remainingTimeText.SetPosition(95, 245)
    
    def CorrectImage(self):
        """Handle correct image selection - maintains original API"""
        self.CorrectAnswer()
    
    def WrongImage(self):
        """Handle wrong image selection - maintains original API"""
        self.WrongAnswer()
    
    def _handle_wrong_answer(self):
        """Handle wrong answer for find different CAPTCHA"""
        super(FindTheDifferentCaptcha, self)._handle_wrong_answer()
        # Regenerate images after wrong answer
        self._create_image_grid()
        self._create_different_image()
    
    def GetRandomNum(self, folderList=False):
        """Get random number - maintains original API"""
        if folderList:
            return app.GetRandom(0, 13)
        return app.GetRandom(0, 1)


class SortTheNumbers(BaseCaptcha):
    """Sort the numbers CAPTCHA where user drags numbers to correct order"""
    
    PATH_IMAGE = "zaris/securityvalues/"
    PATH_BUTTON = "offlineshop/lightwork/searchshop/"
    
    def Constructor(self):
        """Constructor method - maintains original API"""
        super(SortTheNumbers, self).Constructor()
        self._initialize_sort_attributes()
    
    def _initialize_sort_attributes(self):
        """Initialize attributes specific to sort numbers CAPTCHA"""
        self.numberParentList = [0, 0, 0, 0, 0]
        self.numberList = [0, 0, 0, 0, 0]
        self.answerNumberList = [0, 0, 0, 0, 0]
        self.numberGetLocalPositionList = [0, 0, 0, 0, 0]
        self.newList = []
        self.isSorted = False
        self.selectedNumber = -1
        self.checkTime = 0
        self.questionRandom = 0
        self.bgImage = None
        self.questionText = None
        self.confirmButton = None
    
    def _generate_challenge(self):
        """Generate new sort challenge"""
        self.isSorted = False
        self.newList = []
        
        self._generate_unique_numbers()
        self._create_number_images()
    
    def _generate_unique_numbers(self):
        """Generate 5 unique random numbers"""
        self.answerNumberList = []
        for gen in xrange(5):
            self.answerNumberList.append(app.GetRandom(0, 9))
        
        # Ensure uniqueness
        self.answerNumberList = set(self.answerNumberList)
        while len(self.answerNumberList) < 5:
            self.answerNumberList.add(app.GetRandom(0, 9))
        self.answerNumberList = list(self.answerNumberList)
    
    def _create_number_images(self):
        """Create draggable number images"""
        for number in xrange(5):
            self.numberList[number] = ui.ExpandedImageBox()
            self.numberList[number].SetParent(self)
            self.numberList[number].SetPosition(75 + number * 36, 155)
            
            image_path = self.PATH_IMAGE + "{}/{}.dds".format(
                self.answerNumberList[number], app.GetRandom(0, 7)
            )
            self.numberList[number].LoadImage(image_path)
            
            self.numberList[number].OnMouseOverIn = self._create_mouse_over_handler(number)
            self.numberList[number].Show()
            self.numberList[number].AddFlag("movable")
    
    def _create_mouse_over_handler(self, index):
        """Create mouse over handler with correct index closure"""
        return lambda: self.OverInNumber(index)
    
    def OverInNumber(self, idx):
        """Handle mouse over number - maintains original API"""
        self.selectedNumber = idx
    
    def LoadWindow(self):
        """Load window UI - maintains original API"""
        self.SetSize(350, 275)
        self.SetCenterPosition()
        
        self._create_background()
        self._create_question_text()
        self._create_number_parents()
        self._create_confirm_button()
        self._setup_common_ui_elements()
        self._position_timer_elements()
    
    def _create_background(self):
        """Create background elements"""
        self.bgImage = ui.ThinBoard()
        self.bgImage.SetParent(self)
        self.bgImage.SetSize(330, 255)
        self.bgImage.SetPosition(10, 10)
        self.bgImage.Show()
    
    def _create_question_text(self):
        """Create question text"""
        self.questionText = ui.TextLine()
        self.questionText.SetParent(self)
        self.questionText.SetPosition(38, 20)
        
        self.questionRandom = app.GetRandom(0, 10)
        sort_type = localeInfo.SECURITY_SORT_IMAGES2 if self.questionRandom < 5 else localeInfo.SECURITY_SORT_IMAGES3
        self.questionText.SetText(localeInfo.SECURITY_SORT_IMAGES.format(sort_type))
        self.questionText.Show()
    
    def _create_number_parents(self):
        """Create parent containers for numbers"""
        for parent in xrange(5):
            self.numberParentList[parent] = ui.ThinBoardCircle()
            self.numberParentList[parent].SetParent(self)
            self.numberParentList[parent].SetSize(50, 70)
            self.numberParentList[parent].SetPosition(30 + 60 * parent, 50)
            self.numberParentList[parent].Show()
    
    def _create_confirm_button(self):
        """Create confirm button"""
        self.confirmButton = ui.MakeButton(
            self, 92, 130, False, "offlineshop/lightwork/searchshop/",
            "btn_filter_norm.png", "btn_filter_hover.png", "btn_filter_down.png"
        )
        self.confirmButton.SetText(localeInfo.SECURITY_CONFIRM)
        self.confirmButton.SetEvent(ui.__mem_func__(self.ConfirmSelected))
    
    def _position_timer_elements(self):
        """Position timer elements"""
        if self.remainingTimeGauge:
            self.remainingTimeGauge.SetPosition(42 + 35, 235)
        if self.remainingTimeText:
            self.remainingTimeText.SetPosition(95 + 35, 245)
    
    def ConfirmSelected(self):
        """Confirm selected arrangement - maintains original API"""
        if self._check_all_positions_correct():
            self.CorrectAnswer()
        else:
            chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SECURITY_ANSWER_IS_WRONG)
            self.endTime -= 5
    
    def _check_all_positions_correct(self):
        """Check if all numbers are in correct positions"""
        position_ranges = [
            (30, 48, 39, 60),
            (90, 108, 39, 60),
            (150, 168, 39, 60),
            (210, 228, 39, 60),
            (270, 288, 39, 60)
        ]
        
        for i, (min_x, max_x, min_y, max_y) in enumerate(position_ranges):
            number_index = self.GetNumberImagePosition(i)
            x, y = self.numberList[number_index].GetLocalPosition()
            
            if not (min_x <= x <= max_x and min_y <= y <= max_y):
                return False
        
        return True
    
    def PlaceNearest(self):
        """Place number in nearest valid position - maintains original API"""
        if self.checkTime > app.GetTime():
            return
        
        if self.selectedNumber >= 0 and self.numberList[self.selectedNumber].IsIn():
            self._snap_to_nearest_position()
            self.checkTime = app.GetTime() + 0.2
    
    def _snap_to_nearest_position(self):
        """Snap selected number to nearest valid position"""
        x, y = wndMgr.GetMousePosition()
        
        snap_positions = [
            (656, 703, 39, 51),
            (713, 760, 97, 51),
            (776, 823, 157, 51),
            (835, 882, 218, 51),
            (895, 942, 279, 51)
        ]
        
        if 360 <= y <= 430:
            for min_x, max_x, snap_x, snap_y in snap_positions:
                if min_x <= x <= max_x:
                    self.numberList[self.selectedNumber].SetPosition(snap_x, snap_y)
                    break
    
    def ResetNumberPositions(self):
        """Reset number positions - maintains original API"""
        for number in xrange(5):
            self.numberList[number].SetPosition(75 + number * 36, 155)
    
    def GetRandomNum(self, folderList=False):
        """Get random number - maintains original API"""
        if folderList:
            return app.GetRandom(0, 13)
        return app.GetRandom(0, 1)
    
    def SortTheList(self):
        """Sort the answer list - maintains original API"""
        if self.isSorted:
            return
        
        self.newList = list(self.answerNumberList)
        
        # Sort based on question type (ascending or descending)
        for i in xrange(len(self.newList)):
            for j in xrange(i + 1, len(self.newList)):
                should_swap = (
                    (self.newList[i] > self.newList[j]) if self.questionRandom < 5 
                    else (self.newList[i] < self.newList[j])
                )
                if should_swap:
                    self.newList[i], self.newList[j] = self.newList[j], self.newList[i]
        
        self.isSorted = True
    
    def GetNumberImagePosition(self, idx):
        """Get number image position - maintains original API"""
        self.SortTheList()
        return self.answerNumberList.index(self.newList[idx])
    
    def OnUpdate(self):
        """Update method - maintains original API"""
        super(SortTheNumbers, self).OnUpdate()
        
        # Update position tracking
        for nm in xrange(5):
            if self.numberList[nm]:
                self.numberGetLocalPositionList[nm] = [self.numberList[nm].GetLocalPosition()]


# Utility functions for CAPTCHA management
def CreateWritingCaptcha():
    """Factory function to create WritingCaptcha"""
    return WritingCaptcha()


def CreateFindDifferentCaptcha():
    """Factory function to create FindTheDifferentCaptcha"""
    return FindTheDifferentCaptcha()


def CreateSortNumbersCaptcha():
    """Factory function to create SortTheNumbers"""
    return SortTheNumbers()


class CaptchaManager(object):
    """Manager for handling different types of CAPTCHAs"""
    
    CAPTCHA_TYPES = {
        'writing': WritingCaptcha,
        'find_different': FindTheDifferentCaptcha,
        'sort_numbers': SortTheNumbers
    }
    
    def __init__(self):
        self.current_captcha = None
        self.captcha_history = []
    
    def CreateCaptcha(self, captcha_type):
        """Create a CAPTCHA of specified type"""
        if captcha_type not in self.CAPTCHA_TYPES:
            raise ValueError("Unknown CAPTCHA type: %s" % captcha_type)
        
        captcha_class = self.CAPTCHA_TYPES[captcha_type]
        return captcha_class()
    
    def ShowRandomCaptcha(self):
        """Show a random CAPTCHA type"""
        import random
        captcha_type = random.choice(self.CAPTCHA_TYPES.keys())
        self.current_captcha = self.CreateCaptcha(captcha_type)
        self.current_captcha.Open()
        self.captcha_history.append(captcha_type)
        return self.current_captcha
    
    def CloseCurrent(self):
        """Close current CAPTCHA"""
        if self.current_captcha:
            self.current_captcha.Close()
            self.current_captcha = None
    
    def GetCurrentCaptcha(self):
        """Get current CAPTCHA instance"""
        return self.current_captcha
		