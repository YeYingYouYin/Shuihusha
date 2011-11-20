-- TitianXingDao Shuihusha part 4.

return {
	["BWQZ"] = "博闻强识",

	["#_houjian"] = "通臂猿",
	["houjian"] = "侯健",
	["designer:houjian"] = "宇文天启",
	["yuanyin"] = "援引",
	[":yuanyin"] = "你可以将其他角色装备区里的武器当【杀】、非武器当【闪】使用或打出",
	["yuanyin:slash"] = "你想发动技能【援引·杀】吗？",
	["yuanyin:jink"] = "你想发动技能【援引·闪】吗？",

	["#_mengkang"] = "玉幡竿",
	["mengkang"] = "孟康",
	["zaochuan"] = "造船",
	[":zaochuan"] = "出牌阶段，你可以将你的任一锦囊牌当【铁索连环】使用或重铸。",
	["mengchong"] = "艨艟",
	[":mengchong"] = "锁定技，武将牌未处于横置状态的角色计算与武将牌处于横置状态的角色的距离时，始终+1。",

	["#_jiaoting"] = "没面目",
	["jiaoting"] = "焦挺",
	["designer:jiaoting"] = "宇文天启",
	["qinlong"] = "擒龙",
	[":qinlong"] = "当你装备区没牌时，使用【杀】可额外指定一个目标且无使用次数限制",

	["#_shantinggui"] = "圣水将",
	["shantinggui"] = "单廷珪",
	["designer:shantinggui"] = "宇文天启",
	["xiaofang"] = "消防",
	[":xiaofang"] = "当场上出现火焰伤害时，你可以弃掉一张手牌，将其改为无属性伤害",
	["#Xiaofang"] = "%from 发动了技能【%arg】，消除了 %to 受到伤害的火焰属性",

	["#_jiashi"] = "毒蔷薇",
	["jiashi"] = "贾氏",
	["cv:jiashi"] = "呼呼",
	["coder:jiashi"] = "凌天翼",
	["banzhuang"] = "半妆",
	[":banzhuang"] = "出牌阶段，你可以将你的任一红桃手牌当【无中生有】使用",
	["$banzhuang1"] = "一顾倾人城，再顾倾人国。",
	["$banzhuang2"] = "虚事难入公门，实事难以抵对。",
	["zhuying"] = "朱樱",
	[":zhuying"] = "锁定技，你的【酒】均视为【桃】。",
	["$zhuying1"] = "奴家这厢有礼了～",
	["$zhuying2"] = "奴家不胜酒力，浅饮一杯，聊表敬意。",

-- last words
	["~jiashi"] = "员外，饶了奴家吧～",

--
	["gongsunzan"] = "公孙瓒", 
	["yicong"] = "义从", 
	[":yicong"] = "锁定技，只要你的体力值大于2点，你计算与其他角色的距离时，始终-1；只要你的体力值为2点或更低，其他角色计算与你的距离时，始终+1。",
	
	["yuanshu"] = "袁术",
	["yongsi"] = "庸肆",
	[":yongsi"] = "锁定技，摸牌阶段，你额外摸X张牌，X为场上现存势力数。弃牌阶段，你至少弃掉等同于场上现存势力数的牌（不足则全弃）",
	["weidi"] = "伪帝",
	[":weidi"] = "锁定技，你视为拥有当前主公的主公技。",
	
	["tuoqiao"] = "脱壳",
	["xuwei"] = "续尾",
	[":tuoqiao"] = "金蝉脱壳，变身为SP貂蝉",
	[":xuwei"] = "狗尾续貂，变身为原版貂蝉",
	
	["#YongsiGood"] = "%from 的锁定技【庸肆】被触发，额外摸了 %arg 张牌",
	["#YongsiBad"] = "%from 的锁定技【庸肆】被触发，必须至少弃掉 %arg 张牌",
	["#YongsiWorst"] = "%from 的锁定技【庸肆】被触发，弃掉了所有的装备和手牌（共 %arg 张）",
	
	["taichen"] = "抬榇",
	[":taichen"] = "出牌阶段，你可以自减1点体力或弃一张武器牌，弃掉你攻击范围内的一名角色处（手牌、装备区、判定区）的两张牌，每回合中，你可以多次使用抬榇",
	["$taichen"] = "良将不惧死以苟免，烈士不毁节以求生",
	["~sp_pangde"] = "吾宁死于刀下，岂降汝乎",
	
	["cv:gongsunzan"] = "",
	["cv:yuanshu"] = "名将三国",
	["cv:sp_sunshangxiang"] = "",
	["cv:sp_diaochan"] = "",
	["cv:sp_pangde"] = "Glory",
	
	["designer:sp_pangde"] = "太阳神上",
	
	["$yongsi1"] = "嘿呀，还不错",
	["$yongsi2"] = "呐~哈哈哈哈哈",
	["$yongsi3"] = "呀呀呀呀呀呀",
	["$yongsi4"] = "嗙啪~呜哈哈哈哈哈",
	["~yuanshu"] = "呃呀~~~~~~~",
	
	["~gongsunzan"] = "如今阵败吾已无颜苟活于世",
        
	["sp_sunshangxiang"] = "SP孙尚香",
	["chujia"] = "出嫁",
	[":chujia"] = "<b>限定技</b>, 游戏开始时，你可以选择变身为SP孙尚香，势力为蜀",

	["sp_pangde"] = "SP庞德",
	["sp_diaochan"] = "SP貂蝉",
        
	["sp_guanyu"] = "SP关羽",
	["danji"] = "单骑",
	[":danji"] = "<b>觉醒技</b>，回合开始阶段，若你的手牌数大于你当前的体力值，且本局主公为曹操时，你须减1点体力上限并永久获得技能“马术”。",
	["#DanjiWake"] = "%from 的手牌数(%arg)多于体力值(%arg2)，且本局主公为曹操，达到【单骑】的觉醒条件",
	
	["sp_diaochan"] = "SP貂蝉",
	["tuoqiao"] = "脱壳",
	["xuwei"] = "续尾",
	[":tuoqiao"] = "金蝉脱壳，变身为SP貂蝉",
	[":xuwei"] = "狗尾续貂，变身技</b>，游戏开始时，你可以选择变身为SP蔡文姬，势力为魏",
	
	["sp_machao"] = "SP马超",
--sp_card	
	["sp_cards"] = "SP卡牌包",
	["sp_moonspear"] = "SP银月枪", 
	[":sp_moonspear"] = "你的回合外，若打出了一张黑色的牌，你可以立即指定攻击范围内的一名角色打出一张【闪】，否则受到你对其造成的1点伤害", 
	["@moon-spear-jink"] = "受到SP银月枪技能的影响，你必须打出一张【闪】", 
}
