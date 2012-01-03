sgs.ai_keep_value = {
	Shit = 6,

	Peach = 5,

	Analeptic = 4.5,
	Jink = 4,

	Nullification = 3,

	Slash = 2,
	ThunderSlash = 2.5,
	FireSlash = 2.6,

	ExNihilo=4.6,

	AmazingGrace=-1,
	Lightning=-1,

	Ecstasy = 1.5,
	Drivolt = 1,
	Counterplot = 3.2,
	Wiretap = -1,
	Assassinate = 1.8,
	Provistore = 0,
	Treasury = -1,
	Tsunami = -1,
}

sgs.ai_use_value =
{

--skill cards
	XinzhanCard = 4.4,
	TianyiCard = 8.5,
	XianzhenCard = 9.2,
	XianzhenSlashCard = 9.2,
	HuangtianCard = 8.5,
	JijiangCard=8.5,
	DimengCard=3.5,
	JujianCard=6.7,
	QiangxiCard=2.5,
	LijianCard=8.5,
	RendeCard=8.5,
	MinceCard=5.9,
	ZhihengCard = 9,

	ShenfenCard = 8,
	GreatYeyanCard = 8,
	MediumYeyanCard = 5.6,
--normal cards
	ExNihilo=10,

	Snatch=9,
	Collateral=7.8,


	Indulgence=8,
	SupplyShortage=7,

	Peach = 6,
	Dismantlement=5.6,
	IronChain = 5.4,

	--retain_value=5

	FireAttack=4.8,


	FireSlash = 4.4,
	ThunderSlash = 4.5,
	Slash = 4.6,

	ArcheryAttack=3.8,
	SavageAssault=3.9,
	Duel=3.7,

	AmazingGrace=3,
	Nullification = 8.2,

	--special
	Analeptic = 5.98,
	Jink=8.9,
	Shit=-10,

	Ecstasy = 5,
	Drivolt = 5.2,
	Counterplot = 9,
--	Wiretap = 9,
	Assassinate = 8.8,
	Provistore = 1,
	Treasury = 1,
	Tsunami = -2,
	Events = 3,
}

sgs.ai_use_priority = {
--priority of using an active card

--skill cards
	XinzhanCard = 9.2,
	GuhuoCard = 10,
	TianyiCard = 4,
	JieyinCard = 2.5,
	HuangtianCard = 10,
	XianzhenCard = 9.2,
	XianzhenSlashCard = 2.6,
	JijiangCard = 2.4,
	DimengCard=2.3,
	LijianCard = 4,
	QingnangCard=4.2,
	RendeCard= 5.8,
	MingceCard=4,
	JujianCard = 4.5,

	ShenfenCard = 2.3,
	GreatYeyanCard = 9,
	MediumYeyanCard = 6,
	SmallYeyanCard = 2.3,
	JilveCard = 0.4,
--

	Peach = 4.1,

	Dismantlement=4.4,
	Snatch=4.3,
	ExNihilo=4.6,

	GodSalvation=3.9,

	ArcheryAttack=3.5,
	SavageAssault=3.5,


	Duel=2.9,
	IronChain = 2.8,

	Collateral=2.75,

	Analeptic = 2.7,

	OffensiveHorse = 2.69,
	Halberd=2.685,
	KylinBow=2.68,
	Blade = 2.675,
	GudingBlade=2.67,
	DoubleSword =2.665,
	Spear = 2.66,
	Fan=2.655,
	IceSword=2.65,
	QinggangSword=2.645,
	Axe=2.64,
	MoonSpear=2.635,
	Crossbow = 2.63,


	FireSlash = 2.6,
	ThunderSlash = 2.5,
	Slash = 2.4,

	FireAttack=2,
	AmazingGrace=1.0,


	SilverLion=0.9,
	EightDiagram=0.8,
	RenwangShield=0.7,
	Vine=0.6,
	GaleShell=0.9,

	DefensiveHorse = 0,

	Ecstasy = 2.7,
	Drivolt = 3,
--	Counterplot = NULL,
	Wiretap = 4,
	Assassinate = 2.8,
	Provistore = 4.2,
	Treasury = 1,
	Tsunami = 1,
	Events = 3,

	DoubleWhip = 2.64,
	MeteorSword = 2.668,
	SunBow = 2.567,
	GoldArmor = 2.555,
	--god_salvation
	--deluge
	--supply_shortage
	--earthquake
	--indulgence
	--mudslide
	--lightning
	--typhoon
}


-- this table stores the chaofeng value for some generals
-- all other generals' chaofeng value should be 0
sgs.ai_chaofeng = {
	andaoquan = 7,
	shenwuyong = 7,

	shijin = 6,
	yuehe = 6,
	gongsunsheng = 6,

	songjiang = 5,
	lujunyi = 5,
	huangxin = 5,
	houjian = 5,
	kongliang = 5,
	haosiwen = 5,

	peixuan = 4,
	wangqing = 4,
	hantao = 4,
	husanniang = 4,
	yangzhi = 4,
	shiqian = 4,
	jiaoting = 4,
	wangdingliu = 4,
	panjinlian = 4,
	duansanniang = 4,

	luozhenren = 3,
	linchong = 3,
	likui = 3,
	qingzhang = 3,
	baisheng = 3,
	gongwang = 3,
	wangpo = 3,
	baixiuying = 3,
	pengqi = 3,
	jiangsong = 3,
	shenzhangqing = 3,

	luzhishen = 2,
	wusong = 2,
	weidingguo = 2,
	tongguan = 2,
	oupeng = 2,
	yanshun = 2,
	dingdesun = 2,

	mengkang = 1,
	taozongwang = 1,
	zhutong = 1,
	muhong = 1,

	huarong = 0,
	jiashi = 0,
	liying = 0,
	zhangqing = 0,
	shiwengong = 0,

	zhuwu = -2,
	guansheng = -2,
	yanqing = -2,
	chaijin = -2,

	zhaoji = -3,
	panqiaoyun = -3,
	lizhong = -3,
	linniangzi = -4,
	gaoqiu = -4,
	qiaodaoqing = -4,
	zhangshun = -4,
	zhoutong = -5,
	shien = -5,
}

