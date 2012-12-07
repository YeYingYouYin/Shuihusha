-- ZhechongYannan Shuihusha part 8.

local tt = {
	["YBYT"] = "义薄云天",
	["coder:YBYT"] = "战栗贵公子",

	["#baoxu"] = "丧门神",
	["baoxu"] = "鲍旭",
	["cv:baoxu"] = "佚名",
	["sinue"] = "肆虐",
	[":sinue"] = "出牌阶段，每当你杀死一名其他角色，可以弃置一张手牌，对一名距离为1的其他角色造成2点伤害。",
	["@sinue"] = "你可以发动【肆虐】，弃置一张手牌，对一名距离为1的其他角色造成2点伤害",
	["$sinue1"] = "三尸剑下不留人！",
	["$sinue2"] = "汝必死无疑！",
	["~baoxu"] = "被……惩戒了。",
--	["~baoxu"] = "急于抢功可恨误入城池",

	["#xiangchong"] = "八臂哪吒",
	["xiangchong"] = "项充",
	["cv:xiangchong"] = "爪子",
	["xuandao"] = "旋刀",
	[":xuandao"] = "<b>锁定技</b>，当你使用的【杀】被【闪】抵消时，你不能发动武器技能，视为你对目标角色的下家使用了同一张【杀】。\
★同一张【杀】即同来源、同程度、同属性、同花色、同点数。",
	["#Xuandao"] = "%from 的锁定技【%arg】被触发，视为对 %to 使用了同一张【杀】",
	["$xuandao1"] = "刀影漫天，必有一伤！",
	["$xuandao2"] = "飞刀贯心！",
	["~xiangchong"] = "命绝～睦州城～",

	["#jindajian"] = "玉臂匠",
	["jindajian"] = "金大坚",
	["cv:jindajian"] = "卡修【剪刀剧团】",
	["fangzao"] = "仿造",
	[":fangzao"] = "出牌阶段，你可以展示任一其他角色的一张手牌，若该牌为基本牌或非延时类锦囊，则你可以将你的任一手牌当该牌使用。每回合限一次。",
	["jiangxin"] = "匠心",
	[":jiangxin"] = "在任意角色的判定牌生效后，若其为基本牌，你可以摸一张牌。",
	["$fangzao1"] = "精雕细凿，万无一失。",
	["$fangzao2"] = "以假乱真。",
	["$jiangxin1"] = "匠心独运！",
	["$jiangxin2"] = "精华在笔端，咫尺匠心难！",
	["~jindajian"] = "印信雕刻竟纰漏百出，吾愧矣！",

	["#houjian"] = "通臂猿", -- kou 2hp (bwqz)
	["houjian"] = "侯健",
	["designer:houjian"] = "宇文天启",
	["cv:houjian"] = "卜音悠扬【LE秘密】",
	["feizhen"] = "飞针",
	[":feizhen"] = "你可以将其他角色装备区里的武器当【杀】、非武器当【闪】使用或打出。",
	["feizhen:slash"] = "你想发动技能【飞针·杀】吗？",
	["feizhen:jink"] = "你想发动技能【飞针·闪】吗？",
	["$feizhen1"] = "飞针走线，小事一桩。",
	["$feizhen2"] = "针影闪动，战袍已新。",
	["~houjian"] = "蚕丝终究是断了。",

	["#yangchun"] = "白花蛇",
	["yangchun"] = "杨春",
	["cv:yangchun"] = "倔强的小红军【剪刀剧团】",
	["shexin"] = "蛇信",
	[":shexin"] = "出牌阶段，你可以弃置一张非延时类锦囊或装备牌，展示任一其他角色的手牌并弃掉其中除基本牌外的所有牌。每回合限一次。",
	["$shexin1"] = "此毒，无药可解。",
	["$shexin2"] = "辣手摧花！",
	["~yangchun"] = "居然……比我还狠……",

	["#xueyong"] = "病大虫",
	["xueyong"] = "薛永",
	["cv:xueyong"] = "爪子",
	["coder:xueyong"] = "战栗贵公子、宇文天启",
	["maiyi"] = "卖艺",
	[":maiyi"] = "出牌阶段，你可以弃置两张装备牌或三张不同花色的牌，执行下列两项中的一项：1.令任意两名角色各摸两张牌；2.回合结束时令任一角色进行一个额外的回合。每回合限一次。",
	["#MaiyiCanInvoke"] = "%from 发动了技能【%arg】，令 %to 立即进行一个额外的回合",
	["$maiyi1"] = "请诸位捧个人场！",
	["$maiyi2"] = "走过路过，不要错过！",
--	["~xueyong"] = "城管来了，快跑啊……",
	["~xueyong"] = "遇到高手了！",

	["#tanglong"] = "金钱豹子",
	["tanglong"] = "汤隆",
	["designer:tanglong"] = "宇文天启",
	["cuihuo"] = "淬火",
	[":cuihuo"] = "当你失去装备区里的一张牌时，可以摸两张牌",
	["goldsoup"] = "金汤",
	[":goldsoup"] = "<b>锁定技</b>，当你受到装备有武器的角色造成的伤害时，有30%的反伤几率；你每装备一件装备，反伤几率增加15%",
	["~tanglong"] = "祸起萧墙……破金汤……",

	["#zouyuan"] = "出林龙",
	["zouyuan"] = "邹渊",
	["cv:zouyuan"] = "猎狐【声声melody】",
	["longao"] = "龙傲",
	[":longao"] = "当其他角色使用非延时类锦囊指定了唯一的目标角色时，你可以弃置一张牌，执行下列两项中的一项：1.将该锦囊转移给除该角色和目标角色外的任一角色；2.弃掉该角色的一张牌。",
	["longao:zhuan"] = "转移锦囊目标",
	["longao:qi"] = "弃掉使用者的一张牌",
	["$longao1"] = "龙出山林，翱翔八荒！",
	["$longao2"] = "这般手段，可不适用。",
	["~zouyuan"] = "今夜山寨里，梦魂安得空回去？",

	["#zhufu"] = "笑面虎",
	["zhufu"] = "朱富",
	["cv:zhufu"] = "明哲【剪刀剧团】",
	["coder:zhufu"] = "战栗贵公子、宇文天启",
	["hunjiu"] = "浑酒",
	[":hunjiu"] = "你可以将你的一张【肉】、【酒】或【迷】当【酒】或【迷】使用。",
	["guitai"] = "鬼胎",
	[":guitai"] = "你的回合外，每当其他角色成为【肉】的目标时，若你已受伤，你可以弃置一张红桃牌，将该【肉】转移给你。",
	["@guitai"] = "%src 吃了【肉】，你可以弃掉一张红桃牌发动【鬼胎】，将【肉】抢走并吃掉",
	["#Guitai"] = "%from 发动了技能【%arg】，将 %to 的 %arg2 抢走吃掉了",
	["$hunjiu1"] = "这梁山义酒如何？",
	["$hunjiu2"] = "客官，这酒可浑？",
	["$guitai1"] = "嘿嘿～",
	["$guitai2"] = "且慢！这可是毒药！",
	["~zhufu"] = "难道笑也有错？",

}

local gongzi = {"baoxu", "xiangchong", "jindajian",
		"yangchun", "zouyuan"}

for _, player in ipairs(gongzi) do
	tt["coder:" .. player] = tt["coder:YBYT"]
end

return tt