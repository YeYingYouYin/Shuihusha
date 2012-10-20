-- translation for HarePackage
-- Fourth of the twelve

return {
	["hare"] = "秃纸",

	["$haosiwen"] = "041",
	["#haosiwen"] = "井木犴", -- guan 4hp (zcyn)
	["haosiwen"] = "郝思文",
	["illustrator:haosiwen"] = "swell膨",
	["cv:haosiwen"] = "雨过天青【华人之声】",
	["sixiang"] = "四象",
	[":sixiang"] = "回合开始时，你可以弃置一张牌，令至多X名角色（至少一名）将手牌调整至X张（X为场上现存势力数）。若如此做，则弃牌阶段，你至少须弃置等同于场上现存势力数的牌（不足则全弃）。",
	["#SixiangBad"] = "%from 受到技能【%arg2】影响，必须至少弃掉 %arg 张牌",
	["#SixiangWorst"] = "%from 受到技能【%arg2】影响，弃掉了所有的装备和手牌（共 %arg 张）",
	["@sixiang"] = "你可以弃一张牌发动技能【四象】",
	["$sixiang1"] = "青龙、白虎、朱雀、玄武！",
	["$sixiang2"] = "朱雀神鸟，为我先导！",
	["~haosiwen"] = "井宿八星，都陨落了。",

	["$weidingguo"] = "045",
	["#weidingguo"] = "神火将", -- jiang 3hp (xzdd)
	["weidingguo"] = "魏定国",
	["illustrator:weidingguo"] = "封神演义前传",
	["cv:weidingguo"] = "叶恒渊【神蛙教】",
	["coder:weidingguo"] = "凌天翼",
	["fenhui"] = "焚毁",
	[":fenhui"] = "<b>锁定技</b>，你造成的无属性伤害均视为火焰伤害；当你受到火焰伤害时，防止该伤害。",
	["#FenhuiFire"] = "%from 的锁定技【%arg2】被触发，该伤害视为火焰伤害",
	["#FenhuiProtect"] = "%from 的锁定技【%arg2】被触发，防止了 %arg 点火焰伤害",
	["shenhuo"] = "神火",
	[":shenhuo"] = "出牌阶段，你可以将你的任一♥或♦锦囊牌当【火攻】使用；当你使用【火攻】时，（在结算前）你可以摸两张牌。",
	["$fenhui1"] = "烈焰升腾，万物尽毁！", --1和2为1效果
	["$fenhui2"] = "（燃烧声）",
	["$fenhui3"] = "火神附体！", --3和4为2效果
	["$fenhui4"] = "此火伤不了我！",
	["$shenhuo1"] = "东风起，大火生！", --1和2是1效果
	["$shenhuo2"] = "以火应敌，贼人何处逃窜？",
	["$shenhuo3"] = "看我焚诀秘法！", --3是2效果
	["~weidingguo"] = "玩火自焚啊！",

	["$xiaorang"] = "046",
	["#xiaorang"] = "圣手书生", -- min 3hp (cgdk)
	["xiaorang"] = "萧让",
	["illustrator:xiaorang"] = "L.魚儿 ",
	["cv:xiaorang"] = "卡修【天子会工作室】",
	["coder:xiaorang"] = "宇文天启、战栗贵公子",
	["linmo"] = "临摹",
	[":linmo"] = "当其他角色对你使用的基本牌或非延时类锦囊结算完毕进入弃牌堆时，你可以将其置于你的武将牌上，称为“字”；出牌阶段，你可以将你的任一手牌当任意一张“字”使用。每阶段限一次；回合结束时，你须将所有的“字”移入弃牌堆。",
	["zi"] = "字",
	["zhaixing"] = "摘星",
	[":zhaixing"] = "在任意角色的判定牌生效前，若其为♦，则你可以用你的一张牌替换之，然后你摸一张牌。",
	["@zhaixing-card"] = "请使用【摘星】技能来修改 %src 的 %arg 判定",
	["$linmo1"] = "苏、黄、米、蔡？看我写来！", --1和2是1效果
	["$linmo2"] = "妙笔无声！",
	["$linmo3"] = "青衫鸟帽气棱棱，顷刻龙蛇笔底生。", --3和4是2效果
	["$linmo4"] = "雪花飘扬，各有其名！",
	["$linmo5"] = "花团锦簇，文笔生辉。", --5是3效果（弃“字”）
	["$zhaixing1"] = "圣手摘星！",
	["$zhaixing2"] = "读书破万卷，下笔如有神。",
	["~xiaorang"] = "心已无声，生命，可否仿造？",

	["$peixuan"] = "047",
	["#peixuan"] = "铁面孔目", -- guan 3hp (xzdd)
	["peixuan"] = "裴宣",
	["illustrator:peixuan"] = "仙剑神曲",
	["cv:peixuan"] = "倔强的小红军【剪刀剧团】",
	["shenpan"] = "审判",
	[":shenpan"] = "在任意角色的判定牌生效前，你可以获得该判定牌并令该角色重新进行一次判定且判定结果立即生效。",
	["binggong"] = "秉公",
	[":binggong"] = "回合开始时，若你的手牌数大于你的当前体力值，则你可以将等于两者差数的手牌以任意分配方式交给其他角色。若你以此法给出的牌不少于三张，则你回复1点体力。",
	["@binggong"] = "你可以发动【秉公】，将你手牌数与当前体力值差数的手牌交给其他角色",
	["$shenpan1"] = "我既收得财物，自当给你机会！",
	["$shenpan2"] = "阴阳双判，定生死！",
	["$binggong1"] = "可知寨中规矩？",
	["$binggong2"] = "定要赏罚分明。",
	["~peixuan"] = "尽是～暗箱操作！",

	["$ligun"] = "064",
	["#ligun"] = "飞天大圣", -- jiang 4hp (zcyn)
	["ligun"] = "李衮",
	["cv:ligun"] = "大宝今天见【淅雨听风】",
	["illustrator:ligun"] = "冯戈",
	["hengchong"] = "横冲",
	[":hengchong"] = "当你使用的【杀】被【闪】抵消时，若你的装备区里没有武器牌，则你可以弃置一张与该【杀】花色相同的牌，令该【杀】依然造成伤害，然后你对目标角色的上家或下家造成1点伤害。",
	["@hengchong"] = "你对 %src 使用的【杀】为 %arg 花色，可以弃置与其相同花色的手牌发动【横冲】",
	["#Hengchong"] = "%from 发动了【%arg】，弃置了 %arg2 令该【杀】依然对 %to 造成伤害",
	["$hengchong1"] = "看吾直撞军心！",
	["$hengchong2"] = "汝休想苟活！",
	["$hengchong3"] = "横冲营的兄弟，随我杀！",
	["~ligun"] = "冲不出去了！",

	["$tongwei"] = "068",
	["#tongwei"] = "出洞蛟", -- min 3hp (cgdk)
	["tongwei"] = "童威",
	["cv:tongwei"] = "鸢飞【天子会工作室】",
	["illustrator:tongwei"] = "黄玉郎",
	["dalang"] = "大浪",
	[":dalang"] = "你可以跳过你的判定阶段和摸牌阶段，将任一角色判定区里任意数量的牌分别移动到另一名角色区域内的相应位置。",
	["dalangfrom"] = "请选择一名判定区不为空的角色",
	["dalangto"] = "请选择这张延时锦囊要转移的目的地",
	["qianshui"] = "潜水",
	[":qianshui"] = "<b>锁定技</b>，没有装备武器的其他角色使用的【杀】和【行刺】对你无效。",
	["$dalang1"] = "无风亦起浪！",
	["$dalang2"] = "瞒天过海，以待天时。",
	["$qianshui1"] = "蛟龙入海，汝奈我何？",
	["$qianshui2"] = "你可敢入水与我一战？",
	["~tongwei"] = "祗看后浪推前浪。",

	["$songqing"] = "076",
	["#songqing"] = "铁扇子", -- min 3hp (ybyt)
	["songqing"] = "宋清",
	["cv:songqing"] = "烨子风暴【天子会工作室】",
	["illustrator:songqing"] = "侠义世界",
	["coder:songqing"] = "战栗贵公子、宇文天启",
	["sheyan"] = "设宴",
	[":sheyan"] = "出牌阶段，你可以将你的任一♥手牌当【五谷丰登】使用。每阶段限一次。",
	["jiayao"] = "佳肴",
	[":jiayao"] = "任意角色使用【五谷丰登】从牌堆顶亮出牌时，你可以摸X+1张牌并回复X点体力（X为亮出的牌中【肉】和【酒】的数量之和）。",
	["$sheyan1"] = "诸位兄弟，请！",
	["$sheyan2"] = "让吾款待则个。",
	["$jiayao1"] = "嗯～好酒！",
	["$jiayao2"] = "美酒佳肴，岂不妙哉？",
	["~songqing"] = "天下哪有不散的宴席。",

	["$dingdesun"] = "079",
	["#dingdesun"] = "中箭虎", -- jiang 6hp (bwqz)
	["dingdesun"] = "丁得孙",
	["designer:dingdesun"] = "烨子&宇文天启",
	["cv:dingdesun"] = "东方胤弘【天子会工作室】",
	["illustrator:dingdesun"] = "懒惰虫",
	["coder:dingdesun"] = "凌天翼",
	["fushang"] = "负伤",
	[":fushang"] = "<b>锁定技</b>，你每受到一次【杀】造成的伤害，若你的体力上限大于3，则你须扣减1点体力上限，然后摸三张牌。",
	["$fushang1"] = "小伤而已。",
	["$fushang2"] = "沙场征战，难免负伤。",
	["~dingdesun"] = "呃～毒气～攻心！",

	["$songwan"] = "082",
	["#songwan"] = "云里金刚", -- kou 4hp
	["songwan"] = "宋万",
	["cv:songwan"] = "Sin柠檬【月玲珑】",
	["illustrator:songwan"] = "黄玉郎",
	["coder:songwan"] = "Slob",
	["yijie"] = "义节",
	[":yijie"] = "出牌阶段，你可以失去1点体力，令任一角色摸两张牌。每阶段限一次；当你死亡时，你可以将你的所有牌交给任一其他角色，然后令其回复1点体力。",
	["$yijie1"] = "弓弩丛中逃性命，刀枪林里救英雄。", --1和2是1效果
	["$yijie2"] = "相貌语言，南北东西虽各别；心情肝胆，忠诚信义并无差。",
	["$yijie3"] = "千里面朝夕相见，一寸心生死可同。",  --3和4是2效果
	["$yijie4"] = "兄弟，可要好生护着山寨啊。",
	["~songwan"] = "义到尽头原是命啊！",

	["$zhoutong"] = "087",
	["#zhoutong"] = "小霸王", -- kou 3hp (ttxd)
	["zhoutong"] = "周通",
	["cv:zhoutong"] = "疯狂牛魔王【风行天下】",
	["coder:zhoutong"] = "roxiel",
	["illustrator:zhoutong"] = "佚名",
	["qiangqu"] = "强娶",
	[":qiangqu"] = "当你使用【杀】对已受伤的女性角色造成伤害时，你可以防止该伤害，获得该角色的一张牌，然后你与其各回复1点体力。",
	["#Qiangqu"] = "%from 硬是把 %to 拉入了洞房",
	["huatian"] = "花田",
	[":huatian"] = "你每受到1点伤害，可以令任一已受伤的其他角色回复1点体力；你每回复1点体力，可以对任一其他角色造成1点伤害。",
	["$qiangqu1"] = "小娘子，春宵一刻值千金啊！",  --1和2发动技能播放
	["$qiangqu2"] = "今夜，本大王定要做新郎！",
	["$qiangqu3"] = "帽儿光光，今夜做个新郎；衣衫窄窄，今夜做个娇客。",  --3和4为周通回复体力时播放，满血不播放
	["$qiangqu4"] = "我明日要你做压寨夫人！",
	["$huatian1"] = "无妨，只当为汝披嫁纱！", --1和2是1效果（为别人回复体力）
	["$huatian2"] = "只要娘子开心，怎样都好！",
	["$huatian3"] = "破晓之前，忘了此错。", --3和4是2效果（对别人造成伤害）
	["$huatian4"] = "无心插柳，岂是花田之错？",
	["~zhoutong"] = "虽有霸王相，奈无霸王功啊！",

	["$zhugui"] = "092",
	["#zhugui"] = "旱地忽律", -- kou 3/4hp
	["zhugui"] = "朱贵",
	["cv:zhugui"] = "大宝今天见【淅雨听风】",
	["illustrator:zhugui"] = "王者世界",
	["coder:zhugui"] = "roxiel",
	["shihao"] = "施号",
	[":shihao"] = "摸牌阶段，你可以少摸一张牌，视为你对任一其他角色使用一张【探听】。若如此做，回合结束时，你观看牌堆顶的一张牌，然后交给任一角色。",
	["laolian"] = "老练",
	[":laolian"] = "你妹使用一张【肉】或【酒】，可以视为你对任一其他角色使用一张【迷】（该【迷】不计入每回合使用限制）。",
	["$shihao1"] = "此是山寨里的号箭，少刻便有船来。",  -- 1和2发动技能时播放
	["$shihao2"] = "一支响箭穿云霄！",
	["$shihao3"] = "令箭一起，消息立去。",  -- 3是回合结束摸牌播放
	["$laolian1"] = "黑吃黑，鬼咬鬼！",
	["$laolian2"] = "来，来，来，吃些酒肉再走不迟。",
	["$laolian3"] = "欲求生快活，须下死工夫！",
	["~zhugui"] = "祸福无门，惟人自招。",

	["$zhaoji"] = "142",
	["#zhaoji"] = "宋徽宗", -- guan 3hp (god)
	["zhaoji"] = "赵佶",
	["designer:zhaoji"] = "烨子&群龙令",
	["cv:zhaoji"] = "烨子风暴【天子会工作室】",
	["illustrator:zhaoji"] = "帝国传奇",
	["coder:zhaoji"] = "青苹果",
	["shemi"] = "奢靡",
	[":shemi"] = "你可以弃置一张牌跳过你的弃牌阶段，将你的武将牌翻面；当你的武将牌被翻面时，你可以摸等同于你已损失的体力值的牌（至少为1且至多为2）。",
	["@shemi"] = "你可以发动【奢靡】，弃一张牌跳过弃牌阶段并将武将牌翻面。",
	["lizheng"] = "离政",
	[":lizheng"] = "<b>锁定技</b>，若你的武将牌背面向上，则当其他角色计算与你的距离时，始终+1。",
	["nongquan"] = "弄权",
	[":nongquan"] = "<font color=red><b>主公技</b></font>，其他官势力角色可以跳过其摸牌阶段，令你将你的武将牌翻面。",
	["$shemi1"] = "琼林玉殿，朝喧弦管，暮列笙琶。",
	["$shemi2"] = "皓月如昼，共乘欢，争忍归来。",
	["$nongquan1"] = "哈哈～就依爱卿之见！",
	["$nongquan2"] = "退朝，退朝。",
	["~zhaoji"] = "家山回首三千里，目断山南无雁飞。",

}
