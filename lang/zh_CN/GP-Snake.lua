-- translation for SnakePackage
-- Sixth of the twelve

return {
	["snake"] = "巳蛇",

	["$muhong"] = "024",
	["#muhong"] = "没遮拦", -- jiang 4hp (ttxd)
	["muhong"] = "穆弘",
	["illustrator:muhong"] = "楚河汉界",
	["cv:muhong"] = "Emiya【心语联盟】",
	["coder:muhong"] = "roxiel",
	["wuzu"] = "无阻",
	[":wuzu"] = "<b>锁定技</b>，当你计算与其他角色的距离时，-X（X为你已损失的体力值且至少为1）；你始终无视其他角色的防具。",
	["#wuzu_distance"] = "无阻",
	["$IgnoreArmor"] = "%to 装备着 %card，但 %from 貌似没有看见",
	["$wuzu1"] = "谁敢挡我？",
	["$wuzu2"] = "游击部，冲！",
	["~muhong"] = "弟，兄先去矣。",

	["$oupeng"] = "048",
	["#oupeng"] = "摩云金翅", -- jiang 4.5hp (qjwm)
	["oupeng"] = "欧鹏",
	["illustrator:oupeng"] = "黄玉郎",
	["designer:oupeng"] = "烨子&凌天翼",
	["cv:oupeng"] = "烨子风暴【天子会工作室】",
	["zhanchi"] = "展翅",
	[":zhanchi"] = "<font color=purple><b>限定技</b></font>，你可以跳过你的判定阶段并弃掉你判定区里的所有牌，永久获得技能“腾飞”（<b>锁定技</b>，回合结束后，你须扣减1点体力上限，进行一个额外的回合；你的攻击范围等于你的当前体力值）。",
	["@wings"] = "翅",
	["tengfei"] = "腾飞",
	[":tengfei"] = "<b>锁定技</b>，回合结束时，你须减1点体力上限，进行一个额外的回合；你的攻击范围等于你的当前体力值。",
	["#Tengfei"] = "%from 的锁定技【%arg】被触发，将进行一个额外的回合",
	["$zhanchi"] = "迦楼罗，重生！",
	["$tengfei1"] = "我要飞得更高！",
	["$tengfei2"] = "以吾之血，毕其功于一役！",
	["$tengfei3"] = "一冲到底！",
	["~oupeng"] = "呃～啊！连珠箭！",

	["$lingzhen"] = "052",
	["#lingzhen"] = "轰天雷", -- jiang 4hp (zcyn)
	["lingzhen"] = "凌振",
	["illustrator:lingzhen"] = "黑山老妖",
	["cv:lingzhen"] = "鬼叔【月影配音社团】",
	["coder:lingzhen"] = "凌天翼",
	["paohong"] = "炮轰",
	[":paohong"] = "<b>锁定技</b>，你的♠和♣普通【杀】均视为具雷电属性的【杀】；你使用具雷电属性的【杀】时无距离限制。",
	["$paohong1"] = "一炮就送你归天！",
	["$paohong2"] = "不投降就去死吧！",
	["~lingzhen"] = "呃～我绝不会倒下～",

	["$baoxu"] = "060",
	["#baoxu"] = "丧门神", -- kou 4hp (ybyt)
	["baoxu"] = "鲍旭",
	["illustrator:baoxu"] = "nassery3",
	["cv:baoxu"] = "鬼叔【月影配音社团】",
	["coder:baoxu"] = "战栗贵公子",
	["sinue"] = "肆虐",
	[":sinue"] = "出牌阶段，你每杀死一名其他角色，可以弃置一张手牌，对任一距离为1的其他角色造成2点伤害。",
	["@sinue"] = "你可以发动【肆虐】，弃置一张手牌，对一名距离为1的其他角色造成2点伤害",
	["$sinue1"] = "三尸剑下不留人！",
	["$sinue2"] = "看俺再宰一个！",
	["$sinue3"] = "哼！统统见阎王去吧！",
	["~baoxu"] = "被惩戒了！",

	["$fanrui"] = "061",
	["#fanrui"] = "混世魔王", -- kou 3hp (cgdk)
	["fanrui"] = "樊瑞",
	["illustrator:fanrui"] = "大唐豪侠",
	["designer:fanrui"] = "烨子&宇文天启",
	["cv:fanrui"] = "莫名【忆昔端华工作室】",
	["hunyuan"] = "混元",
	[":hunyuan"] = "你每受到1点伤害或回复1点体力，可以亮出牌堆顶的一张牌，然后将其置于你的武将牌上，称为“熵”；在任意角色的判定牌生效前，你可以弃置一张“熵”替换之。",
	["wudao"] = "悟道",
	[":wudao"] = "<font color=green><b>觉醒技</b></font>，回合结束阶段开始时，若“熵”不少于三张，则你须将体力上限调整至2，摸两张牌，并永久获得技能“奇门”和“空门”（<b>锁定技</b>，当你失去最后的手牌时，你回复1点体力）。",
	["#WudaoWake"] = "%from 将体力上限调整为 %arg",
	["kongmen"] = "空门",
	[":kongmen"] = "<b>锁定技</b>，每当你失去最后一张手牌时，你回复1点体力。",
	["shang"] = "熵",
	["$hunyuan1"] = "天之道，损有余而补不足。",
	["$hunyuan2"] = "孰魔孰道，问天自明。",
	["$wudao"] = "洗心讵悬解，悟道正迷津。",
	["$kongmen1"] = "道生一，一生二，二生三，三生万物。",
	["$kongmen2"] = "人法地，地法天，天法道，道法自然。",
	["$qimen3"] = "生门为生，死门为死。",
	["$qimen4"] = "立盘天地人，三奇日月星。",
	["~fanrui"] = "吉凶自有天数。",

	["$jindajian"] = "066",
	["#jindajian"] = "玉臂匠", -- kou 3hp (ybyt)
	["jindajian"] = "金大坚",
	["illustrator:jindajian"] = "大话西游",
	["cv:jindajian"] = "卡修【天子会工作室】",
	["coder:jindajian"] = "战栗贵公子",
	["fangzao"] = "仿造",
	[":fangzao"] = "出牌阶段，你可以展示任一其他角色的一张手牌：若该牌为基本牌或非延时类锦囊，则你可以将你的任一手牌当该牌使用。每阶段限一次。",
	["jiangxin"] = "匠心",
	[":jiangxin"] = "在任一角色的判定牌生效后，若其为基本牌，则你可以摸一张牌。",
	["$fangzao1"] = "精雕细凿，万无一失。",
	["$fangzao2"] = "以假乱真。",
	["$jiangxin1"] = "锤凿之间，匠心独妙，别具一格。",
	["$jiangxin2"] = "精华在笔端，咫尺匠心难！",
	["~jindajian"] = "印信雕刻竟纰漏百出，吾愧矣！",

	["$houjian"] = "071",
	["#houjian"] = "通臂猿", -- kou 2hp (bwqz)
	["houjian"] = "侯健",
	["illustrator:houjian"] = "烨子风暴",
	["designer:houjian"] = "宇文天启",
	["cv:houjian"] = "Emiya【心语联盟】",
	["feizhen"] = "飞针",
	[":feizhen"] = "你可以将其他角色装备区里的武器牌当【杀】、防具牌或坐骑牌当【闪】使用或打出。",
	["feizhen:slash"] = "你想发动技能【飞针·杀】吗？",
	["feizhen:jink"] = "你想发动技能【飞针·闪】吗？",
	["tiancan"] = "天蚕",
	[":tiancan"] = "回合结束阶段结束时，你可以摸一张牌。",
	["$feizhen1"] = "飞针走线，小事一桩。",
	["$feizhen2"] = "针影闪动，战袍已新。",
	["$tiancan1"] = "绣得旌旗振军威！",
	["$tiancan2"] = "缝得战袍壮士气！",
	["~houjian"] = "战袍虽新，只可惜～唉！",

	["$xuanzan"] = "040",
	["#xuanzan"] = "丑郡马", -- guan 4hp
	["xuanzan"] = "宣赞",
	["illustrator:xuanzan"] = "彭超",
	["cv:xuanzan"] = "东方胤弘【天子会工作室】",
	["konghe"] = "恐吓",
	[":konghe"] = "其他角色的回合开始时，若你的武将牌背面向上，则你可以将你的武将牌翻面，令该角色跳过其该回合的判定阶段和任一其他阶段（回合开始阶段和回合结束阶段除外）；你每受到一次伤害，若你的武将牌正面向上，则你可以将你的武将牌翻面。",
	["konghe:k1"] = "跳过摸牌阶段",
	["konghe:k2"] = "跳过出牌阶段",
	["konghe:k3"] = "跳过弃牌阶段",
	["$konghe1"] = "小的们，截了他们的粮！", --（前半部分）（跳摸牌）
	["$konghe2"] = "不谋其兵，先挫其势。",
	["$konghe3"] = "要你吓出一身冷汗！", --（跳出牌）
	["$konghe4"] = "待我先唬住他。",
	["$konghe5"] = "敌将勇猛，兄弟不可急进啊！", --（跳弃牌）
	["$konghe6"] = "保存实力，以备不时之需。",
	["$konghe7"] = "吾这般模样，见不得人呐！", --（后半部分）
	["$konghe8"] = "小的们，风紧扯呼～",
	["~xuanzan"] = "此处便是饮马桥了。",

	["$dengfei"] = "049",
	["#dengfei"] = "火眼狻猊", -- kou 4hp
	["dengfei"] = "邓飞",
	["illustrator:dengfei"] = "枫",
	["cv:dengfei"] = "雅策君【忆籁闻声】",
	["jiejiu"] = "截救",
	[":jiejiu"] = "当任一角色受到来源不为你的无属性伤害时，你可以和伤害来源拼点。若你赢，则你获得对方拼点的牌且防止该伤害。若你没赢，则你不能发动“截救”，直到你的下一个回合开始。 ",
	["@jiejiu"] = "%src 对 %dest 即将造成伤害，你可以发动【截救】与 %src 拼点",
	["#Jiejiu"] = "%from 成功【%arg】了 %to",
	["@block"] = "退避",
	["$jiejiu1"] = "兄弟先走，待我挡住来人！",
	["$jiejiu2"] = "都让开！", --（赢）
	["$jiejiu3"] = "好厉害的本事！", --（输）
	["~dengfei"] = "索大哥，快走～",

	["$huangfuduan"] = "057",
	["#huangfuduan"] = "紫髯伯", -- min 3hp
	["huangfuduan"] = "皇甫端",
	["illustrator:huangfuduan"] = "三国鼎立",
	["cv:huangfuduan"] = "大宝今天见【淅雨听风】",
	["xiangma"] = "相马",
	[":xiangma"] = "当任一角色进入濒死状态时，你可以弃掉除该角色外的任一角色装备区里的一张坐骑牌，视为你对该濒死角色使用一张【肉】。",
	["@xiangma"] = "你可以发动【相马】，弃掉一角色的坐骑牌救援 %src",
	["yima"] = "医马",
	[":yima"] = "当其他角色的坐骑牌进入弃牌堆时，你可以获得之，然后可以令该角色回复1点体力。",
	["yima:yes"] = "令其回复体力",
	["yima:no"] = "不回复",
	["$xiangma1"] = "相得好马，赠予良将！",
	["$xiangma2"] = "有此良驹，实为将军之福也。",
	["$yima1"] = "好一匹枣红马！",
	["$yima2"] = "医得一马，救得一人。",
	["~huangfuduan"] = "赠以良驹与英雄，可惜百战空余鞍！",

	["$suyuanjing"] = "154",
	["#suyuanjing"] = "文曲星", -- guan 3hp
	["suyuanjing"] = "宿元景",
	["illustrator:suyuanjing"] = "战国风云",
	["cv:suyuanjing"] = "Sin柠檬【月玲珑】",
	["zhaoan"] = "招安",
	[":zhaoan"] = "出牌阶段，你可以指定任一与你势力不同的其他角色。若其交给你一张【杀】或武器牌，则其摸两张牌，否则其不能使用【酒】和【肉】，直到回合结束。每阶段限一次。",
	["@zhaoan"] = "%src 对你进行了【招安】，你须交给 %src 一张【杀】或武器牌，否则不能使用酒肉直到回合结束",
	["#Zhaoan"] = "%from 受到【%arg】效果影响，无法使用肉和酒直到回合结束",
	["fuxu"] = "抚恤",
	[":fuxu"] = "任意角色每受到一次【杀】造成的伤害，你可以令其进行一次判定：若结果为♠或♣，则其获得该判定牌且你摸一张牌；若结果为♥或♦，则你可以弃置一张♥或♦手牌，令其回复1点体力。",
	["@fuxu"] = "你可以弃置一张红色手牌发动【抚恤】，令 %src 回复一点体力",
	["$zhaoan1"] = "宿元景愿奉君命，特赍御酒到此。",
	["$zhaoan2"] = "众头领有此归顺之心，实乃我大宋之福也！", --（目标摸牌）
	["$zhaoan3"] = "尔等莫不是要负隅顽抗？", --（目标不给）
	["$fuxu1"] = "将军尽忠之心，吾定会禀报天子！", --（黑色效果）
	["$fuxu2"] = "下官当保诸位英雄莫拆散分开。",
	["$fuxu3"] = "先锋莫忧，元景回朝，天子前必当重保。", --（红色效果）
	["$fuxu4"] = "元景先饮此杯，与众义士看！",
	["~suyuanjing"] = "天不垂怜，吾大宋必将亡于奸佞之手！",

	["$liangshijie"] = "162",
	["#liangshijie"] = "梁中书", -- guan 3hp
	["liangshijie"] = "梁世杰",
	["illustrator:liangshijie"] = "cabbage0316",
	["cv:liangshijie"] = "雅策君【忆籁闻声】",
	["sougua"] = "搜刮",
	[":sougua"] = "你可以跳过你的摸牌阶段并将你的武将牌翻面，分别获得一至四名其他角色的一张手牌。",
	["@sougua"] = "你可以发动【搜刮】，获得一至四名其他角色的一张手牌并将自己翻面",
	["liushou"] = "留守",
	[":liushou"] = "<b>锁定技</b>，若你的武将牌背面向上，则【杀】对你无效；你的回合外，你每失去一张装备区里的牌，若你的武将牌背面向上，则你须将其翻面。",
	["$sougua1"] = "岳丈大人生辰将至，待我筹备这生辰纲！",
	["$sougua2"] = "管他们是死是活，这可是要献给太师的贺礼！",
	["$sougua3"] = "一张、两张，哼，还是不够！",
	["$liushou1"] = "城高墙厚，暂保无虞！", --（前半部分）
	["$liushou2"] = "给我守住，给我守住！",
	["$liushou3"] = "都杀进来了，还不走！", --（后半部分）
	["$liushou4"] = "守不住了，快走！",
	["~liangshijie"] = "噗！",
}
