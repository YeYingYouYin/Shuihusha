-- translation for SnakePackage
-- Sixth of the twelve

return {
	["snake"] = "巳蛇",

	["$muhong"] = "024",
	["#muhong"] = "没遮拦", -- jiang 4hp (ttxd)
	["muhong"] = "穆弘",
	["cv:muhong"] = "流岚【裔美声社】",
	["coder:muhong"] = "roxiel",
	["wuzu"] = "无阻",
	[":wuzu"] = "<b>锁定技</b>，当你计算与其他角色的距离时，-X（X为你已损失的体力值且至少为1）；你始终无视其他角色的防具。",
	["#wuzu_distance"] = "无阻",
	["$IgnoreArmor"] = "%to 装备着 %card，但 %from 貌似没有看见",
	["$wuzu1"] = "谁敢拦我？",
	["$wuzu2"] = "游击部，冲！",
	["~muhong"] = "弟，兄先去矣！",

	["$oupeng"] = "048",
	["#oupeng"] = "摩云金翅", -- jiang 4.5hp (qjwm)
	["oupeng"] = "欧鹏",
	["zhanchi"] = "展翅",
	["designer:oupeng"] = "烨子&凌天翼",
	["cv:oupeng"] = "烨子【剪刀剧团】",
	[":zhanchi"] = "<font color=purple><b>限定技</b></font>，你可以跳过你的判定阶段并弃掉你判定区里的所有牌，永久获得技能“腾飞”（<b>锁定技</b>，回合结束后，你须扣减1点体力上限，进行一个额外的回合；你的攻击范围等于你的当前体力值）。",
	["@wings"] = "翅",
	["tengfei"] = "腾飞",
	[":tengfei"] = "<b>锁定技</b>，回合结束时，你须减1点体力上限，进行一个额外的回合；你的攻击范围等于你的当前体力值。",
	["#Tengfei"] = "%from 的锁定技【%arg】被触发，将进行一个额外的回合",
	["$zhanchi"] = "迦楼罗，重生！",
	["$tengfei1"] = "我要飞得更高！",
	["$tengfei2"] = "以吾之血，毕其功于一役！",
	["~oupeng"] = "呃～啊！连珠箭！",

	["$lingzhen"] = "052",
	["#lingzhen"] = "轰天雷", -- jiang 4hp (zcyn)
	["lingzhen"] = "凌振",
	["cv:lingzhen"] = "佚名",
	["coder:lingzhen"] = "凌天翼",
	["paohong"] = "炮轰",
	[":paohong"] = "<b>锁定技</b>，你的♠和♣普通【杀】均视为具雷电属性的【杀】；你使用具雷电属性的【杀】时无距离限制。",
	["$paohong1"] = "一炮就送你归天！",
	["$paohong2"] = "不投降就去死吧！",
	["~lingzhen"] = "啊！我不会倒下～",

	["$baoxu"] = "060",
	["#baoxu"] = "丧门神", -- kou 4hp (ybyt)
	["baoxu"] = "鲍旭",
	["cv:baoxu"] = "佚名",
	["coder:baoxu"] = "战栗贵公子",
	["sinue"] = "肆虐",
	[":sinue"] = "出牌阶段，你每杀死一名其他角色，可以弃置一张手牌，对任一距离为1的其他角色造成2点伤害。",
	["@sinue"] = "你可以发动【肆虐】，弃置一张手牌，对一名距离为1的其他角色造成2点伤害",
	["$sinue1"] = "三尸剑下不留人！",
	["$sinue2"] = "汝必死无疑！",
	["~baoxu"] = "被……惩戒了。",
--	["~baoxu"] = "急于抢功可恨误入城池",

	["$fanrui"] = "061",
	["#fanrui"] = "混世魔王", -- kou 3hp (cgdk)
	["fanrui"] = "樊瑞",
	["cv:fanrui"] = "烨子【剪刀剧团】",
	["kongmen"] = "空门",
	[":kongmen"] = "<b>锁定技</b>，每当你失去最后一张手牌时，你回复1点体力。",
	["wudao"] = "悟道",
	[":wudao"] = "<font color=green><b>觉醒技</b></font>，任意角色的回合开始时，若你没有手牌，你须摸两张牌，然后减1点体力上限，并永久获得技能“卜天”和“奇门”。",
	["$kongmen1"] = "身合天地，自有乾坤！",
	["$kongmen2"] = "道生一，一生二，二生三，三生万物。",
	["$wudao"] = "洗心讵悬解，悟道正迷津。",
	["$butian3"] = "天之道，损有余而补不足。",
	["$butian4"] = "幽冥之间，万物已循因缘。",
	["$qimen3"] = "奇门道法，毁天灭地。",
	["$qimen4"] = "生门为生，死门为死。",
	["~fanrui"] = "吉凶自有天数。",

	["$jindajian"] = "066",
	["#jindajian"] = "玉臂匠", -- kou 3hp (ybyt)
	["jindajian"] = "金大坚",
	["cv:jindajian"] = "卡修【剪刀剧团】",
	["coder:jindajian"] = "战栗贵公子",
	["fangzao"] = "仿造",
	[":fangzao"] = "出牌阶段，你可以展示任一其他角色的一张手牌：若该牌为基本牌或非延时类锦囊，则你可以将你的任一手牌当该牌使用。每阶段限一次。",
	["jiangxin"] = "匠心",
	[":jiangxin"] = "在任一角色的判定牌生效后，若其为基本牌，则你可以摸一张牌。",
	["$fangzao1"] = "精雕细凿，万无一失。",
	["$fangzao2"] = "以假乱真。",
	["$jiangxin1"] = "匠心独运！",
	["$jiangxin2"] = "精华在笔端，咫尺匠心难！",
	["~jindajian"] = "印信雕刻竟纰漏百出，吾愧矣！",

	["$houjian"] = "071",
	["#houjian"] = "通臂猿", -- kou 2hp (bwqz)
	["houjian"] = "侯健",
	["designer:houjian"] = "宇文天启",
	["cv:houjian"] = "卜音悠扬【LE秘密】",
	["feizhen"] = "飞针",
	[":feizhen"] = "你可以将其他角色装备区里的武器牌当【杀】、防具牌或坐骑牌当【闪】使用或打出。",
	["feizhen:slash"] = "你想发动技能【飞针·杀】吗？",
	["feizhen:jink"] = "你想发动技能【飞针·闪】吗？",
	["$feizhen1"] = "飞针走线，小事一桩。",
	["$feizhen2"] = "针影闪动，战袍已新。",
	["~houjian"] = "蚕丝终究是断了。",

	["$xuanzan"] = "040",
	["#xuanzan"] = "丑郡马", -- guan 4hp
	["xuanzan"] = "宣赞",
	["konghe"] = "恐吓",

	["$dengfei"] = "049",
	["#dengfei"] = "火眼狻猊", -- kou 4hp
	["dengfei"] = "邓飞",
	["jiejiu"] = "截救",

	["$huangfuduan"] = "057",
	["#huangfuduan"] = "紫髯伯", -- min 3hp
	["huangfuduan"] = "皇甫端",
	["xiangma"] = "相马",
	[":xiangma"] = "当任一角色进入濒死状态时，你可以弃掉除该角色外的任一角色装备区里的一张坐骑牌，视为你对该濒死角色使用一张【肉】。",
	["yima"] = "医马",
	[":yima"] = "当其他角色的坐骑牌进入弃牌堆时，你可以获得之并可以令该角色回复1点体力。",

	["$suyuanjing"] = "154",
	["#suyuanjing"] = "宿太尉", -- guan 3hp
	["suyuanjing"] = "宿元景",

	["$liangshijie"] = "162",
	["#liangshijie"] = "梁中书", -- guan 3hp
	["liangshijie"] = "梁世杰",

}
