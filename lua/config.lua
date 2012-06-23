
-- this script to store the basic configuration for game program itself
-- and it is a little different from config.ini

config = {
	version = "20120621",
	version_name = "终结版F1.7",
	mod_name = "Shuihusha",
	kingdoms = { "guan", "jiang", "min", "kou", "god"},
	package_names = {

	"StandardCard",
	"Plough",
	"Maneuvering",
	"ExCard",
	"Events",
	"Gift",
--	"CustomCard",
--	"Kuso",
--	"Joy",

	"Standard",
	"Rat",
--[[	"QJWM",
	"XZDD",
	"BWQZ",
	"QLFD",
	"ZCYN",
	"CGDK",
	"YBYT",
	"FCDC",
	"God",
	"Stanley",
	"InterChange",]]
	"SP",
	"Test",

--	"JoyGeneral",
	},

	scene_names = {
	"Couple",
--[[	"Zombie",
	"Legend",
	"Impasse",]]
	"Custom",
	},

	color_guan = "#547998",
	color_jiang = "#D0796C",
	color_min = "#4DB873",
	color_kou = "#8A807A",
	color_god = "#96943D",
}

for i=1, 20 do
	local scene_name = ("MiniScene_%02d"):format(i)
	table.insert(config.scene_names, scene_name)
end

