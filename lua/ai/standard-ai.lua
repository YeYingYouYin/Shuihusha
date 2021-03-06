-- songjiang
sgs.ai_chaofeng.songjiang = -2
sgs.songjiang_suit_value =
{
	heart = 3.9,
}
sgs.songjiang_keep_value =
{
	Jink = 5.1,
}

-- ganlin
sgs.ai_card_intention.GanlinCard = function(card, from, tos, self)
	self:speakTrigger(card,from,tos[1])
	sgs.updateIntentions(from, tos, -70)
end
sgs.ai_use_value.GanlinCard = 8.5
sgs.ai_use_priority.GanlinCard = 4.8
sgs.dynamic_value.benefit.GanlinCard = true

sgs.ai_skill_invoke["ganlin"] = function(self, data)
	return self.player:getHandcardNum() < 2 and self.player:isWounded()
end
local ganlin_skill={}
ganlin_skill.name = "ganlin"
table.insert(sgs.ai_skills, ganlin_skill)
ganlin_skill.getTurnUseCard = function(self)
	if self.player:hasFlag("Ganlin") or self.player:isKongcheng() then return end
	if self.player:getHandcardNum() == 1 then
		if self.player:isWounded() and not self:getSun2niang() then
			return sgs.Card_Parse("@GanlinCard=.")
		else
			return
		end
	end
	if self:slashIsAvailable() and self:getCardsNum("Slash") > 0 then return end
	for _, player in ipairs(self.friends_noself) do
		if ((self:hasSkills("butian|qimen|longluo", player) or player:containsTrick("supply_shortage", false))
			or (not player:containsTrick("indulgence", false) and (self:hasSkills("banzhuang|shouge", player)))
			)
			and player:faceUp() then
			return sgs.Card_Parse("@GanlinCard=.")
		end
	end
	if (self.player:usedTimes("GanlinCard") < 2 or self:getOverflow() > 0 or self:getCard("Shit")) then
		return sgs.Card_Parse("@GanlinCard=.")
	end
	if self.player:getLostHp() < 2 then
		return sgs.Card_Parse("@GanlinCard=.")
	end
end
sgs.ai_skill_use_func["GanlinCard"] = function(card, use, self)
	local cards = sgs.QList2Table(self.player:getHandcards())
	self:sortByUseValue(cards,true)
	local name = self.player:objectName()
	if #self.friends > 1 then
		local shijin, shiqian, wangding6, weidingguo, fanrui
		shijin = self.room:findPlayerBySkillName("xiagu")
		shiqian = self.room:findPlayerBySkillName("shentou")
		wangding6 = self.room:findPlayerBySkillName("kongying")
		weidingguo = self.room:findPlayerBySkillName("shenhuo")
		fanrui = self.room:findPlayerBySkillName("kongmen")
		for _, hcard in ipairs(cards) do
			if not hcard:isKindOf("Shit") then
				if hcard:isKindOf("Analeptic") or hcard:isKindOf("Peach") then
					self:sort(self.friends_noself, "hp")
					if #self.friends>1 and self.friends_noself[1]:getHp() == 1 then
						use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
						if use.to then use.to:append(self.friends_noself[1]) end
						return
					end
				end
				self:sort(self.friends_noself, "hp")
				local friend = self.friends_noself[1]
				if fanrui and fanrui:isWounded() and fanrui:isKongcheng() and self:isFriend(fanrui) and  fanrui:objectName() ~= name then
					use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
					if use.to then use.to:append(fanrui) end
					return
				end
				if wangding6 and self:isFriend(wangding6) and hcard:isKindOf("Jink") and wangding6:objectName() ~= name and
					self:getCardsNum("Jink")>1 then
					use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
					if use.to then use.to:append(wangding6) end
					return
				end
				if shijin and self:isFriend(shijin) and hcard:isKindOf("EquipCard") and shijin:objectName() ~= name then
					use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
					if use.to then use.to:append(shijin) end
					return
				end
				if shiqian and self:isFriend(shiqian) and hcard:getSuit() == sgs.Card_Club and shiqian:objectName() ~= name then
					use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
					if use.to then use.to:append(shiqian) end
					return
				end
				if weidingguo and self:isFriend(weidingguo) and hcard:isKindOf("TrickCard") and hcard:isRed() and weidingguo:objectName() ~= name and
					not (weidingguo:containsTrick("indulgence", false) and not hcard:isKindOf("Nullification")) then
					use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
					if use.to then use.to:append(weidingguo) end
					return
				end
				if self:getUseValue(hcard)<6 and #self.friends>1 then
					for _, friend in ipairs(self.friends_noself) do
						if sgs[friend:getGeneralName() .. "_suit_value"] and
							(sgs[friend:getGeneralName() .. "_suit_value"][hcard:getSuitString()] or 0)>=3.9 then
							use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
							if use.to then use.to:append(friend) end
							return
						end
						if friend:getGeneral2Name()~="" then
							if sgs[friend:getGeneral2Name() .. "_suit_value"] and
								(sgs[friend:getGeneral2Name() .. "_suit_value"][hcard:getSuitString()] or 0)>=3.9 then
								use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
								if use.to then use.to:append(friend) end
								return
							end
						end
						if hcard:getNumber()>10 and self:hasSkills("dalei|taolue|zhongzhen|suocai", friend) then
							use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
							if use.to then use.to:append(friend) end
							return
						end
					end
				end
				if hcard:isKindOf("Armor") then
					self:sort(self.friends_noself, "defense")
					local v = 0
					local target
					for _, friend in ipairs(self.friends_noself) do
						if not friend:getArmor() and self:evaluateArmor(hcard, friend) > v and not friend:containsTrick("indulgence", false) then
							v = self:evaluateArmor(hcard, friend)
							target = friend
						end
					end
					if target then
						use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
						if use.to then use.to:append(target) end
						return
					end
				end
				if hcard:isKindOf("EquipCard") then
					self:sort(self.friends_noself)
					for _, friend in ipairs(self.friends_noself) do
						if not self:getSameEquip(hcard, friend) or friend:hasSkill("xiagu")
							or (self:hasSkills("feiqiang|cuihuo|yinlang|huxiao", friend) and not friend:containsTrick("indulgence", false))  then
							use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
							if use.to then use.to:append(friend) end
							return
						end
					end
				end
			else
				self:sort(self.enemies, "hp")
				for _, enemi in ipairs(self.enemies) do
					if not self:hasSkills("ganlin|binggong|baoguo|haoshen", enemi) then
						use.card = sgs.Card_Parse("@GanlinCard=" .. hcard:getId())
						if use.to then use.to:append(enemi) end
						return
					end
				end
			end
		end
	end

--[[
	local shit
	shit = self:getCard("Shit")
	if shit then
		use.card = sgs.Card_Parse("@GanlinCard=" .. shit:getId())
		self:sort(self.enemies,"hp")
		if use.to then use.to:append(self.enemies[1]) end
		return
	end]]

	if #self.friends == 1 then return end

	if (self:getOverflow()>0 or (self.player:isWounded() and self.player:usedTimes("GanlinCard") > 2))
		then
		self:sort(self.friends_noself, "handcard")
		local friend
		if friend then
			local card_id = self:getCardRandomly(self.player, "h")
			use.card = sgs.Card_Parse("@GanlinCard=" .. card_id)
			if use.to then use.to:append(friend) end
			return
		end
	end

	if self.player:getHandcardNum() == 1 and not self.player:isWounded() then
		for _, enemy in ipairs(self.enemies) do
			if self:isEquip("GudingBlade", enemy) and enemy:canSlash(self.player, true) then return end
		end
	end

	if self.player:getLostHp() < 2 then
		local card
		self:sortByUseValue(cards, true)
		for _, hcard in ipairs(cards) do
			if hcard:isKindOf("Jink") then if self:getCardsNum("Jink")>#self.enemies/2 then card = hcard end
			elseif hcard:isKindOf("Slash") then if self:getCardsNum("Slash") > 1 then card = hcard end
			elseif not hcard:isKindOf("Nullification") then card = hcard end
			if card then break end
		end
		if card then
			self:sort(self.friends_noself, "handcard")
			for _, friend in ipairs(self.friends_noself) do
				local draw = 2
				if friend:containsTrick("supply_shortage", false) then draw = 0 end
				if friend:getHandcardNum() + draw < friend:getMaxCards() and not friend:containsTrick("indulgence", false) and (not self:isWeak(friend) and self:hasSkills(sgs.need_kongcheng,friend)) then
					use.card = sgs.Card_Parse("@GanlinCard=" .. card:getId())
					if use.to then use.to:append(friend) end
					return
				end
			end
			self:sort(self.friends_noself, "handcard")
			for _, friend in ipairs(self.friends_noself) do
				if not friend:containsTrick("indulgence", false) then
					use.card = sgs.Card_Parse("@GanlinCard=" .. card:getId())
					if use.to then use.to:append(self.friends_noself[1]) end
					return
				end
			end
		end
	end
end

-- juyi
sgs.ai_card_intention.JuyiCard = function(card, from, tos)
	local songjiang = from:getRoom():getLord()
	if songjiang and songjiang:getHandcardNum() >= from:getHandcardNum() then
		sgs.updateIntention(from, songjiang, 40)
	else
		sgs.updateIntention(from, songjiang, -50)
	end
end

juyi_skill={}
juyi_skill.name = "jui"
table.insert(sgs.ai_skills, juyi_skill)
juyi_skill.getTurnUseCard = function(self)
	local sj = self.room:getLord()
	if not sj or self.player:getKingdom() ~= "kou" or self.player:hasUsed("JuyiCard") or self:isEnemy(sj) then return end
	local mycardnum = self.player:getHandcardNum()
	local sjcardnum = sj:getHandcardNum()
	if mycardnum - sjcardnum > 1 then
		card = sgs.Card_Parse("@JuyiCard=.")
		return card
	end
end
sgs.ai_skill_use_func["JuyiCard"]=function(card,use,self)
	use.card = card
end
sgs.ai_skill_choice["jui"] = function(self, choice, data)
	local source = data:toPlayer()
	if self:isFriend(source) then
		return "agree"
	else
		return "deny"
	end
end

-- lujunyi
sgs.ai_chaofeng.lujunyi = 5
sgs.lujunyi_keep_value =
{
	BasicCard = 4,
}

-- baoguo
sgs.ai_skill_invoke["baoguo"] = true
sgs.ai_skill_cardask["@baoguo"] = function(self, data)
	if self.player:hasSkill("fushang") and self.player:getHp() > 3 then return "." end
	local damage = data:toDamage()
	local invoke = false
	if damage.damage == 0 then
		invoke = true
	elseif self:isEnemy(damage.to) then
		invoke = false
	elseif damage.to:getHp() <= 2 then
		invoke = true
	else
		invoke = math.random(1, 3) == 2
	end
	if invoke and not self.player:isKongcheng() then
		local pile = self:getCardsNum("Peach") + self:getCardsNum("Analeptic")
		local dmgnum = damage.damage
		if self.player:getHp() + pile - dmgnum > 0 then
			if self.player:getHp() + pile - dmgnum == 1 and pile > 0 then return "." end
			local cards = sgs.QList2Table(self.player:getHandcards())
			self:sortByUseValue(cards, false)
			for _, fcard in ipairs(cards) do
				if fcard:isKindOf("BasicCard") and
					not fcard:isKindOf("Peach") and not fcard:isKindOf("Analeptic") then
					self:speak("baoguo")
					return fcard:getEffectiveId()
				end
			end
		end
	end
	return "."
end

-- wuyong
sgs.ai_chaofeng.wuyong = 6
-- huace
local huace_skill={}
huace_skill.name = "huace"
table.insert(sgs.ai_skills, huace_skill)
huace_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("HuaceCard") or self.player:isKongcheng() then return end
	local cards = sgs.QList2Table(self.player:getHandcards())
	local aoename = "savage_assault|archery_attack"
	local aoenames = aoename:split("|")
	local aoe
	local i
	local good, bad = 0, 0
	local huacetrick = "savage_assault|archery_attack|ex_nihilo|god_salvation"
	local huacetricks = huacetrick:split("|")
	for i=1, #huacetricks do
		local forbiden = huacetricks[i]
		forbid = sgs.Sanguosha:cloneCard(forbiden, sgs.Card_NoSuit, 0)
		if self.player:isLocked(forbid) then return end
	end
	for _, friend in ipairs(self.friends) do
		if friend:isWounded() then
			good = good + 10/(friend:getHp())
			if friend:isLord() then good = good + 10/(friend:getHp()) end
		end
	end
	for _, enemy in ipairs(self.enemies) do
		if enemy:isWounded() then
			bad = bad + 10/(enemy:getHp())
			if enemy:isLord() then
				bad = bad + 10/(enemy:getHp())
			end
		end
	end
	local card = -1
	self:sortByUseValue(cards, true)
	for _, acard in ipairs(cards)  do
		if acard:isKindOf("TrickCard") then
			card = acard:getEffectiveId()
			break
		end
	end
	if card < 0 then return end
	for i=1, #aoenames do
		local newhuace = aoenames[i]
		aoe = sgs.Sanguosha:cloneCard(newhuace, sgs.Card_NoSuit, 0)
		if self:getAoeValue(aoe) > -5 then
			local parsed_card=sgs.Card_Parse("@HuaceCard=" .. card .. ":" .. newhuace)
			return parsed_card
		end
	end
	if good > bad then
		local parsed_card = sgs.Card_Parse("@HuaceCard=" .. card .. ":" .. "god_salvation")
		return parsed_card
	end
	if self:getCardsNum("Jink") == 0 and self:getCardsNum("Peach") == 0 then
		local parsed_card = sgs.Card_Parse("@HuaceCard=" .. card .. ":" .. "ex_nihilo")
		return parsed_card
	end
end
sgs.ai_skill_use_func["HuaceCard"] = function(card, use, self)
	local userstring = card:toString()
	userstring = (userstring:split(":"))[3]
	local huacecard = sgs.Sanguosha:cloneCard(userstring, card:getSuit(), card:getNumber())
	huacecard:setSkillName("huace")
	self:useTrickCard(huacecard,use)
	if not use.card then return end
	use.card = card
end

-- yunchou
sgs.ai_skill_invoke["yunchou"] = true

-- gongsunsheng
-- yixing
function SmartAI:getYixingCard(judge)
	local equips = {}
	if self:getUseValue(judge.card) < 5.5 or judge.card:isKindOf("Jink") or judge.card:isKindOf("Collateral") then
		for _, player in ipairs(self.enemies) do
			local pequips = player:getEquips()
			for _, equip in sgs.qlist(pequips) do
				table.insert(equips, equip)
			end
		end
	end
	for _, player in ipairs(self.friends) do
		local pequips = player:getEquips()
		for _, equip in sgs.qlist(pequips) do
			table.insert(equips, equip)
		end
	end
	if #equips == 0 then return -1 end
	return self:getRetrialCardId(equips, judge)
end
sgs.ai_skill_use["@@yixing"] = function(self, prompt)
	local judge = self.player:getTag("Judge"):toJudge()
	if self:needRetrial(judge) then
		local players = sgs.QList2Table(self.room:getAllPlayers())
		local card_id = self:getYixingCard(judge)
		if card_id < 0 then return "." end
		for _, player in ipairs(players) do
			local pequips = player:getEquips()
			for _, equip in sgs.qlist(pequips) do
				if equip:getEffectiveId() == card_id then
					self.yixingcid = card_id
					if self.player ~= player then self:speak("yixing", player) end
					return "@YixingCard=.->" .. player:objectName()
				end
			end
		end
	end
	return "."
end

-- qimen
sgs.ai_card_intention.QimenCard = 90

function SmartAI:qimenValue(current, target) -- 判断是否有被奇门的必要，参数为当前行动者和被奇门者
--	if current:distanceTo(target) > 2 then return false end
	if self:getZhanghengf(target, current) then return true end
	if target:getMark("@shut") > 0 then return false end
	if current == target then
		if self:hasSkills("ganlin|huace|haoshen|jiashu|yongle|qinxin|shuangzhan|shentou|yuanpei|tongxia|xunlie", target) then
			return true
		end
	else
		if self:hasSkills(sgs.masochism_skill, target) then return true end
		if self:isFriend(current) and target:hasSkill("xiagu") and self:getCardsNum("EquipCard", target) > 0 then return true end
		if self:isEnemy(current) and current:isWounded() and target:hasSkill("jishi") and not target:isKongcheng() then return true end
	end
	return false -- false没必要，true有必要
end
sgs.ai_skill_use["@@qimen"] = function(self, prompt)
	local current = self.room:getCurrent()
	local target
	self:sort(self.enemies, "handcard2")
	for _, tmp in ipairs(self.enemies) do
		if self:qimenValue(current, tmp) then
			target = tmp
			break
		end
	end
	if not target then
		if self:isEnemy(current) and math.random(1, 3) ~= 2 then
			target = current
		end
	end
	if target then
		self.qimentarget = target
		local cards = sgs.QList2Table(self.player:getHandcards())
		self:sortByUseValue(cards, true)
		if self:getUseValue(cards[1]) < 5.7 then
			self:speak("qimen_source")
			return "@QimenCard=" .. cards[1]:getEffectiveId() .. "->" .. target:objectName()
		end
	end
end

-- guansheng
-- tongwu
sgs.ai_skill_invoke["tongwu"] = true
sgs.ai_skill_playerchosen["tongwu"] = function(self, targets)
	if not self:getCard("Jink") then return self.player end
	local targetlist=sgs.QList2Table(targets)
	self:sort(targetlist, "handcard")
	for _, target in ipairs(targetlist) do
		if self:isFriend(target) then
			return target
		end
	end
	return self.player
end

-- linchong
-- duijue
sgs.ai_card_intention.DuijueCard = 80

sgs.ai_skill_use["@@duijue"] = function(self, prompt)
	self:sort(self.enemies, "hp")
	local n1 = self:getCardsNum("Slash")
	local final
	local card = sgs.Sanguosha:cloneCard("duel", sgs.Card_NoSuit, 0)
	for _, enemy in ipairs(self.enemies) do
		if n1 + 1 > self:getCardsNum("Slash", enemy) and self:hasTrickEffective(card, enemy) then
			final = enemy
			break
		end
	end
	if final then
		self:speak("duijue")
		return "@DuijueCard=.->" .. final:objectName()
	else
		return "."
	end
end

-- huarong
-- kaixian
sgs.ai_skill_use["@@kaixian"] = function(self, prompt)
	self:sort(self.enemies)
	for _, enemy in ipairs(self.enemies) do
		local distance = self.player:distanceTo(enemy)
		local cards = sgs.QList2Table(self.player:getHandcards())
		for _, card in ipairs(cards) do
			if card:getNumber() <= 5 and card:getNumber() == distance then
				return "@KaixianCard=" .. card:getEffectiveId() .. "->."
			end
		end
	end
	return "."
end

-- chaijin
-- danshu
sgs.ai_skill_discard["danshu"] = function(self, discard_num, optional, include_equip)
	local to_discard = {}
	local chaijin = self.room:findPlayerBySkillName("danshu")
	self:speak("danshu", chaijin)
	local cards = self.player:getHandcards()
	cards=sgs.QList2Table(cards)
	self:sortByKeepValue(cards, true)
	for _, card in ipairs(cards) do
		if #to_discard >= discard_num then break end
		table.insert(to_discard, card:getId())
	end
	if #to_discard == discard_num then
		return to_discard
	else
		return {}
	end
end
function sgs.ai_slash_prohibit.danshu(self, to)
	local x = to:getLostHp()
	return self.player:getHandcardNum() <= x + 1
end

-- haoshen
sgs.ai_card_intention.HaoshenCard = -70

sgs.ai_skill_use["@@haoshen"] = function(self, prompt)
	if prompt == "@haoshen-draw" and not self.player:isKongcheng() then
		self:sort(self.friends, "handcard")
		local max_x = 2
		local target
		for _, friend in ipairs(self.friends) do
			local x = friend:getMaxHP() - friend:getHandcardNum()
			if x > max_x then
				max_x = x
				target = friend
			end
		end
		if target then
			self:speak("haoshen", target)
			return "@HaoshenCard=.->" .. target:objectName()
		else
			return "."
		end
	elseif prompt == "@haoshen-play" and self.player:getHandcardNum() > 6 then
		self:sort(self.friends_noself, "handcard")
		if #self.friends_noself == 0 then return "." end
		local target = self.friends_noself[1]
		local cards = self.player:getHandcards()
		cards = sgs.QList2Table(cards)
		self:sortByUseValue(cards, true)
		local card_ids = {}
		for i = 1, math.floor((#cards + 1) / 2) do
			table.insert(card_ids, cards[i]:getEffectiveId())
		end
		self:speak("haoshen", target)
		return "@HaoshenCard=" .. table.concat(card_ids, "+") .. "->" .. target:objectName()
	else
		return "."
	end
end

-- zhutong
sgs.zhutong_keep_value =
{
	Peach = 6,
}

-- sijiu
sgs.ai_card_intention.SijiuCard = -100

sijiu_skill={}
sijiu_skill.name = "sijiu"
table.insert(sgs.ai_skills, sijiu_skill)
sijiu_skill.getTurnUseCard = function(self)
	if self:isWeak() then return end
	local peach = self:getCardId("Peach")
	if peach and type(peach) == "number" then
		return sgs.Card_Parse("@SijiuCard=" .. peach)
	end
end
sgs.ai_skill_use_func["SijiuCard"] = function(card, use, self)
	self:sort(self.friends, "hp")
	for _, friend in ipairs(self.friends) do
		if friend:isWounded() then
			use.card = card
			if use.to then
				self:speak("sijiu")
				use.to:append(friend)
			end
			return
		end
	end
end

-- yixian
sgs.ai_skill_invoke["yixian"] = function(self, data)
	local damage = data:toDamage()
	if self:isFriend(damage.to) then
		return not damage.to:getJudgingArea():isEmpty() or not self:hasSkills(sgs.masochism_skill, damage.to)
	elseif self:isEnemy(damage.to) then
		return self:hasSkills(sgs.masochism_skill, damage.to)
	end
end
sgs.ai_skill_cardchosen["yixian"] = function(self, who)
	if self:isFriend(who) and not who:getJudgingArea():isEmpty() then
		return who:delayedTricks():first()
	end
	if self:isEnemy(who) then
		local cards = sgs.QList2Table(who:getCards("he"))
		if #cards > 0 then
			self:sortByUseValue(cards)
			return cards[1]
		end
	elseif self:isFriend(who) then
		local cards = sgs.QList2Table(who:getCards("he"))
		if #cards > 0 then
			self:sortByUseValue(cards, true)
			return cards[1]
		end
	end
	local cards = sgs.QList2Table(who:getCards("hej"))
	return cards[1]
end

-- luzhishen
-- liba
sgs.ai_skill_invoke["liba"] = function(self, data)
	local damage = data:toDamage()
	if self:isEnemy(damage.to) then
		if self.player:hasSkill("liba") then
			self:speak("liba")
		elseif self.player:hasSkill("goulian") then
			self:speak("goulian")
		end
		return true
	else
		return false
	end
end

-- wusong
sgs.wusong_suit_value =
{
	spade = 3.9,
	club = 3.9
}
sgs.wusong_keep_value =
{
	Analeptic = 5.5,
	Weapon = 5,
}

-- fuhu
sgs.ai_skill_cardask["@fuhu"] = function(self, data)
	local damage = data:toDamage()
	if self:slashProhibit(damage.from) then return "." end
	if self:isEnemy(damage.from) then
		local cards = self.player:getCards("he")
		cards = sgs.QList2Table(cards)
		self:sortByUseValue(cards, true)
		local default
		for _, card in ipairs(cards) do
			if card:isBlack() then
				if not default then default = card end
				if self:getCardsNum("Jink", damage.from) == 0 and
					(card:isKindOf("Analeptic") or card:isKindOf("Weapon")) then
					self:speak("fuhu")
					return card:getEffectiveId()
				end
			end
		end
		if default then
			self:speak("fuhu")
			return default:getEffectiveId()
		end
	end
	return "."
end

-- yangzhi
sgs.yangzhi_keep_value =
{
	EventsCard = 6,
}

-- maidao
maidao_skill={}
maidao_skill.name = "maidao"
table.insert(sgs.ai_skills, maidao_skill)
maidao_skill.getTurnUseCard = function(self)
	if self.player:getWeapon() then
		local range = sgs.weapon_range[self.player:getWeapon():className()]
		local cards
		if range < 4 then
			cards = self.player:getCards("he")
		else
			cards = self.player:getCards("h")
		end
		cards = sgs.QList2Table(cards)
		for _, acard in ipairs(cards)  do
			if acard:isKindOf("Weapon") then
				self:speak("maidao")
				return sgs.Card_Parse("@MaidaoCard=" .. acard:getEffectiveId())
			end
		end
	end
	return
end
sgs.ai_skill_use_func["MaidaoCard"] = function(card, use, self)
	use.card = card
end

-- fengmang
sgs.ai_card_intention.FengmangCard = 80

sgs.ai_skill_use["@@fengmang"] = function(self, prompt)
	if #self.enemies == 0 then return end
	self:sort(self.enemies)
	local target = self.enemies[1]
	local cards = self.player:getHandcards()
	local card
	for _, c in sgs.qlist(cards) do
		if c:isKindOf("EventsCard") then
			card = c
			break
		end
	end
	if card then return "@FengmangCard=" .. card:getEffectiveId() .. "->" .. target:objectName()
	else return "@FengmangCard=.->" .. target:objectName()
	end
	return "."
end

-- buyaknife
buyaknife_skill = {}
buyaknife_skill.name = "buyaknife"
table.insert(sgs.ai_skills, buyaknife_skill)
buyaknife_skill.getTurnUseCard = function(self)
	if self.player:getHandcardNum() < 5 then return end
	return sgs.Card_Parse("@BuyaKnifeCard=.")
end
sgs.ai_skill_use_func["BuyaKnifeCard"] = function(card, use, self)
	for _, yangzhi in sgs.qlist(self.room:findPlayersBySkillName("maidao")) do
		if not yangzhi:getPile("knife"):isEmpty() and yangzhi ~= self.player then
			local cards = self.player:getCards("h")
			cards = sgs.QList2Table(cards)
			self:sortByUseValue(cards, true)
			local card_ids = {}
			if self:isEnemy(yangzhi) then
				for _, car in ipairs(cards) do
					if self:getKeepValue(car) < 4.1 and not car:isKindOf("Events") then
						table.insert(card_ids, car:getEffectiveId())
					end
					if #card_ids == 3 then
						use.card = sgs.Card_Parse("@BuyaKnifeCard=" .. table.concat(card_ids, "+"))
						if use.to then
							self:speak("buydao")
							use.to:append(yangzhi)
						end
						return
					end
				end
			else
				if self:getCardsNum("Events") > 1 then
					for _, car in ipairs(cards) do
						if car:isKindOf("Events") then
							table.insert(card_ids, car:getEffectiveId())
						end
						if #card_ids == 3 then
							use.card = sgs.Card_Parse("@BuyaKnifeCard=" .. table.concat(card_ids, "+"))
							if use.to then
								self:speak("buydao")
								use.to:append(yangzhi)
							end
							return
						end
					end
				end
			end
		end
	end
end

-- xuning
-- goulian
sgs.ai_skill_invoke["goulian"] = sgs.ai_skill_invoke["liba"]

-- jinjia
function sgs.ai_armor_value.jinjia(card)
	if not card then return 4 end
end

-- daizong
-- mitan
mitan_skill={}
mitan_skill.name = "mitan"
table.insert(sgs.ai_skills, mitan_skill)
mitan_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("Wiretap") then return end
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	local card
	self:sortByUseValue(cards, true)
	for _,acard in ipairs(cards) do
		if (acard:isKindOf("Lightning") or
			acard:isKindOf("Tsunami") or
			acard:isKindOf("Treasury") or
			acard:isKindOf("Provistore")) then
			card = acard
			break
		end
	end
	if not card then return nil end
	local suit = card:getSuitString()
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()
	local card_str = ("wiretap:mitan[%s:%s]=%d"):format(suit, number, card_id)
	local wire = sgs.Card_Parse(card_str)
	assert(wire)
	return wire
end
sgs.ai_skill_askforag["mitan"] = function(self, card_ids)
	local effect = self.room:getTag("Wiretap"):toCardEffect()
	local cards = {}
	for _, card_id in ipairs(card_ids)  do
		table.insert(cards, sgs.Sanguosha:getCard(card_id))
	end
	if self:isEnemy(effect.to) then
		self:sortByUseValue(cards)
		return cards[1]:getEffectiveId()
	else
		return -1
	end
end

-- jibao
sgs.ai_skill_cardask["@jibao"] = function(self)
	if self.player:getHandcardNum() <= 2 then return "." end
	local cards = self.player:getHandcards()
	cards=sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	self:speak("jibao")
	return cards[1]:getEffectiveId()
end

-- likui
-- shalu
sgs.ai_skill_invoke["shalu"] = function(self, data)
	return not self:getZhangheng(player)
end

-- ruanxiao7
-- jueming
function sgs.ai_trick_prohibit.jueming(card, self, to)
	return to ~= self.room:getCurrent() and to:getHp() == 1 and (card:isKindOf("Duel") or card:isKindOf("Assassinate"))
end
function sgs.ai_slash_prohibit.jueming(self, to)
	if to ~= self.room:getCurrent() and to:getHp() == 1 then return true end
end

-- jiuhan
sgs.ai_skill_invoke["jiuhan"] = function(self, data)
	return self.player:getMaxHp() > 2 and self.player:getHp() < 0
end

-- yangxiong
sgs.yangxiong_suit_value =
{
	spade = 3
}

-- xingxing
sgs.ai_skill_cardask["@xingxing"] = function(self, data)
	local dy = data:toDying()
	if self:isEnemy(dy.who) then
		local cards = self.player:getCards("h")
		for _, card in sgs.qlist(cards) do
			if card:getSuit() == sgs.Card_Spade then
				self:speak("xingxing")
				return card:getEffectiveId()
			end
		end
	end
	return "."
end

-- yanqing
-- dalei
sgs.ai_card_intention.DaleiCard = function(card, from, tos)
	if tos[1]:getHandcardNum() > 3 and tos[1]:getLostHp() == 0 then
		sgs.updateIntentions(from, tos, -30)
	else
		sgs.updateIntentions(from, tos, 70)
	end
end

local dalei_skill={}
dalei_skill.name = "dalei"
table.insert(sgs.ai_skills, dalei_skill)
dalei_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("DaleiCard") or self.player:isKongcheng() then return end
	return sgs.Card_Parse("@DaleiCard=.")
end
sgs.ai_skill_use_func["DaleiCard"] = function(card, use, self)
	local target
	if self.player:getHp() > 1 then
		self:sort(self.enemies, "handcard")
		for _, enemy in ipairs(self.enemies) do
			if not enemy:isKongcheng() and enemy:isMale()
				and self.player:inMyAttackRange(enemy) then
				target = enemy
				break
			end
		end
		local max_card = self:getMaxCard()
		if target and max_card and max_card:getNumber() > 10 then
			use.card = sgs.Card_Parse("@DaleiCard=" .. max_card:getEffectiveId())
			if use.to then use.to:append(target) end
		else
			for _, friend in ipairs(self.friends_noself) do
				if friend:getHandcardNum() > 5 and not friend:isWounded()
					and friend:isMale() then
					target = friend
					break
				end
			end
			local min_num = 14, min_card
			for _, hcard in sgs.qlist(self.player:getHandcards()) do
				if hcard:getNumber() < min_num then
					min_num = hcard:getNumber()
					min_card = hcard
				end
			end
			if target and min_card then
				use.card = sgs.Card_Parse("@DaleiCard=" .. min_card:getEffectiveId())
				if use.to then use.to:append(target) end
			end
		end
	end
end
sgs.ai_skill_invoke["dalei"] = function(self, data)
	local damage = data:toDamage()
	self:sort(self.friends, "hp")
	local caninvoke = false
	for _, friend in ipairs(self.friends) do
		if friend:isWounded() and friend ~= damage.to then
			caninvoke = true
			self.daleirecover = friend
			break
		end
	end
	return caninvoke
end
sgs.ai_skill_playerchosen["dalei"] = function(self, targets)
	return self.daleirecover
end

-- fuqin
sgs.ai_skill_choice["fuqin"] = function(self, choice, data)
	if choice == "qing+nil" then return "qing" end
	local source = data:toDamage().from
	if self:isFriend(source) then
		return "qing"
	else
		local lstn = self.player:getLostHp()
		local canm = source:getCardCount(true)
		if canm >= lstn or self:isWeak() then
			if self:isWeak() then
				self.fuqintarget = 4
				return "qing"
			else
				return "yan"
			end
		else
			return "qing"
		end
	end
end
sgs.ai_skill_playerchosen["fuqin"] = function(self, targets)
	if self.fuqintarget == 4 then return self.player end
	self:sort(self.friends, "handcard")
	if self.friends[1]:getHandcardNum() > 2 then
		self:sort(self.friends, "hp")
		if self.friends[1]:getHp() > 2 then return self.player
		else return self.friends[1] end
	end
	return self.friends[1]
end

-- andaoquan
sgs.andaoquan_keep_value =
{
	TrickCard = 5.5,
	BasicCard = 6,
}

-- jishi
sgs.ai_skill_cardask["@jishi"] = function(self, data)
	local who = data:toPlayer()
	if self:isEnemy(who) or self.player:isKongcheng() then return "." end
	local cards = sgs.QList2Table(self.player:getHandcards())
	self:sortByUseValue(cards, true)
	for _, card in ipairs(cards) do
		if card:isKindOf("TrickCard") or card:isKindOf("BasicCard") then
			self:speak("jishi")
			return card:getEffectiveId()
		end
	end
	return "."
end

-- fengyue
sgs.ai_skill_invoke["fengyue"] = true

-- yanshou
sgs.ai_card_intention.YanshouCard = -100

yanshou_skill={}
yanshou_skill.name = "yanshou"
table.insert(sgs.ai_skills, yanshou_skill)
yanshou_skill.getTurnUseCard = function(self)
	if self.player:getMark("@relic") < 1 then return end
	local cards = self.player:getHandcards()
	local hearts = {}
	cards=sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	for _, card in ipairs(cards) do
		if card:getSuit() == sgs.Card_Heart then
			table.insert(hearts, card:getId())
		end
		if #hearts == 2 then break end
	end
	if #hearts ~= 2 then return end
	return sgs.Card_Parse("@YanshouCard=" .. table.concat(hearts, "+"))
end
sgs.ai_skill_use_func["YanshouCard"]=function(card,use,self)
	self:sort(self.friends, "maxhp")
	for _, friend in ipairs(self.friends) do
		if friend:hasSkill("feizhen") or (friend:hasSkill("yanshou") and not friend:isLord())
			or friend:hasSkill("wudao") then
			use.card = card
			if use.to then
				use.to:append(friend)
			end
			return
		end
	end
	use.card = card
	if use.to then use.to:append(self.friends[1]) end
end

-- hu3niang
-- hongjin
sgs.ai_skill_choice["hongjin"] = function(self, choices, data)
	local who = data:toDamage().to
	if self:isFriend(who) then
		return "draw1card"
	else
		if who:getHandcardNum() == 1 or (who:isKongcheng() and not who:isNude()) then
			return "throw"
		else
			return "draw1card"
		end
	end
end

-- wuji
wuji_skill={}
wuji_skill.name = "wuji"
table.insert(sgs.ai_skills, wuji_skill)
wuji_skill.getTurnUseCard = function(self)
	if self:slashIsAvailable() and self:getCardsNum("Slash") > 0 then return end
	local cards = self.player:getCards("h")
	local slashs = {}
	cards=sgs.QList2Table(cards)
	self:sortByUseValue(cards,true)
	for _,card in ipairs(cards)  do
		if card:isKindOf("Slash") then
			table.insert(slashs, card:getId())
		end
	end
	if #slashs == 0 then return end
	return sgs.Card_Parse("@WujiCard=" .. table.concat(slashs, "+"))
end
sgs.ai_skill_use_func["WujiCard"]=function(card,use,self)
	use.card = card
	self:speak("wuji")
end

-- sun2niang
-- heidian
sgs.ai_skill_cardask["@heidian2"] = function(self)
	self:speak("heidian")
	local ecards = sgs.QList2Table(self.player:getCards("e"))
	if #ecards == 0 then return "." end
	self:sortByUseValue(ecards, true)
	return ecards[1]:getEffectiveId() or "."
end

function SmartAI:getSun2niang(player) -- enemy sun2niang's threat
	player = player or self.player
	local room = player:getRoom()
	local flag = 0
	for _, erniang in sgs.qlist(room:findPlayersBySkillName("heidian")) do
		if erniang ~= player then
			if self:isFriend(player, erniang) and player:hasEquip() then
				flag = flag - 1
			else
				flag = flag + 1
			end
		end
	end
	return flag > 0
end

-- renrou
sgs.ai_skill_invoke["renrou"] = function(self, data)
	local shiti = data:toPlayer()
	local cards = shiti:getHandcards()
	local shit_num = 0
	for _, card in sgs.qlist(cards) do
		if card:isKindOf("Shit") then
			shit_num = shit_num + 1
			if card:getSuit() == sgs.Card_Spade then
				shit_num = shit_num + 1
			end
		end
	end
	return shit_num <= 1
end

-- gaoqiu
-- cuju
sgs.ai_card_intention.CujuCard = 75

sgs.ai_skill_invoke["cuju"] = function(self, data)
	local damage = data:toDamage()
	return damage.damage > 0
end
sgs.ai_skill_use["@@cuju"] = function(self, prompt)
	local gongsunsheng = self.room:findPlayerBySkillName("yixing")
	if self.player:isKongcheng() and (not gongsunsheng or self:isEnemy(gongsunsheng)) then
		return "."
	end
	self:sort(self.enemies, "hp")
	local target = self.enemies[1]
	if not self.player:isKongcheng() then
		local cards = self.player:getHandcards()
		cards = sgs.QList2Table(cards)
		self:sortByUseValue(cards, true)
		if target then
			self:speak("cuju")
			return "@CujuCard=" .. cards[1]:getEffectiveId() .. "->" .. target:objectName()
		end
	else
		return "."
	end
end

-- panquan
sgs.ai_skill_invoke["panquan"] = function(self, data)
	local gaoqiu = self.room:getLord()
	return gaoqiu and self:isFriend(gaoqiu)
end

-- caijing
-- jiashu
sgs.ai_card_intention.JiashuCard = 70

jiashu_skill={}
jiashu_skill.name = "jiashu"
table.insert(sgs.ai_skills, jiashu_skill)
jiashu_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("JiashuCard") or self.player:isKongcheng() then return end
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	self.jiashusuit = cards[1]:getSuitString()
	return sgs.Card_Parse("@JiashuCard=" .. cards[1]:getEffectiveId())
end
sgs.ai_skill_use_func["JiashuCard"] = function(card, use, self)
	local target
	for _, enemy in ipairs(self.enemies) do
		if self:isWeak(enemy) then
			target = enemy
			break
		end
	end
	if not target then
		self:sort(self.enemies, "handcard")
		target = self.enemies[1]
	end
	use.card = card
	if use.to then
		self:speak("jiashu")
		use.to:append(target)
	end
end
sgs.ai_skill_suit["jiashu"] = function(self)
	local map = {}
	if self.jiashusuit ~= "heart" then
		return 2
	else
		map = {0,1,3}
	end
	return map[math.random(1,3)]
end

-- duoquan
function SmartAI:sortBySkillValue(skills, inverse)
	local compare_func = function(a,b)
		local value1 = sgs.ai_skill_value[a] or 0
		local value2 = sgs.ai_skill_value[b] or 0

		if not inverse then return value1 > value2 end
		return value1 < value2
	end

	table.sort(skills, compare_func)
end

sgs.ai_skill_invoke["duoquan"] = function(self, data)
	if self.player:getMark("@power") == 0 then return false end
	local shiti = data:toPlayer()
	local skills = {}
	for _, skill in sgs.qlist(shiti:getVisibleSkillList()) do
		if skill:getLocation() == sgs.Skill_Right and
			skill:getFrequency() ~= sgs.Skill_Limited and
			skill:getFrequency() ~= sgs.Skill_Wake and not skill:isLordSkill() then
			table.insert(skills, skill:objectName())
		end
	end
	if #skills == 0 then return false end
	self:sortBySkillValue(skills)
	local value = sgs.ai_skill_value[skills[1]] or 0
	if (shiti:getHandcardNum() <= 3 and value > 5) or
		(shiti:getHandcardNum() > 3 and value > 4) then
		self.duoquan = skills[1]
		return true
	end
	return false
end
sgs.ai_skill_choice["duoquan"] = function(self, choice, data)
	return self.duoquan
end

-- fangla
-- yongle
sgs.ai_card_intention.YongleCard = 60

local yongle_skill={}
yongle_skill.name = "yongle"
table.insert(sgs.ai_skills, yongle_skill)
yongle_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("YongleCard") then return end
	return sgs.Card_Parse("@YongleCard=.")
end
sgs.ai_skill_use_func["YongleCard"]=function(card,use,self)
	local king = self.room:getKingdoms()
	local enemies = {}
	for _, enemy in ipairs(self.enemies) do
		if not enemy:isKongcheng() then
			table.insert(enemies, enemy)
			if #enemies >= king then break end
		end
	end
	use.card = card
	if use.to then
		self:speak("yongle")
		for _, enemy in ipairs(enemies) do
			use.to:append(enemy)
		end
	end
end
sgs.ai_cardshow["yongle"] = function(self, requestor)
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	return cards[1]
end

-- zhiyuan
sgs.ai_skill_invoke["zhiyuan"] = true
sgs.ai_skill_cardask["@zhiyuan"] = function(self)
	local lord = self.room:getLord()
	if lord and self:isFriend(lord) and not self.player:isKongcheng() then
		local cards = self.player:getCards("h")
		cards=sgs.QList2Table(cards)
		self:sortByUseValue(cards)
		return cards[1]:getEffectiveId()
	elseif self:isEnemy(lord) and lord:hasUsed("YongleCard") then
		local shit = self:getCardId("Shit")
		if shit and type(shit) == "number" then
			return shit
		end
	end
	return "."
end

-- wangqing
-- jiachu
sgs.ai_skill_cardask["@jiachu"] = function(self)
	local target = self.room:getLord()
	if target and self:isFriend(target) then
		local allcards = self.player:getCards("he")
		for _, card in sgs.qlist(allcards) do
			if card:getSuit() == sgs.Card_Heart then
				return card:getEffectiveId()
			end
		end
	end
	return "."
end

-- panjinlian
sgs.panjinlian_suit_value =
{
	heart = 5,
	spade = 4,
}

-- meihuo
sgs.ai_use_priority.MeihuoCard = 2.5
sgs.ai_card_intention.MeihuoCard = -80
sgs.dynamic_value.benefit.MeihuoCard = true

meihuo_skill={}
meihuo_skill.name = "meihuo"
table.insert(sgs.ai_skills, meihuo_skill)
meihuo_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("MeihuoCard") then return end
	local cards = self.player:getCards("h")
	cards = sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	for _, card in ipairs(cards)  do
		if card:getSuit() == sgs.Card_Heart then
			return sgs.Card_Parse("@MeihuoCard=" .. card:getEffectiveId())
		end
	end
	return
end
sgs.ai_skill_use_func["MeihuoCard"] = function(card, use, self)
	for _, friend in ipairs(self.friends) do
		if friend:isMale() and self:isWeak(friend) then
			use.card = card
			if use.to then use.to:append(friend) end
			return
		end
	end
	self:sort(self.friends, "hp")
	for _, friend in ipairs(self.friends_noself) do
		if friend:isMale() and friend:isWounded() then
			use.card = card
			if use.to then use.to:append(friend) end
			return
		end
	end
end

-- zhensha
sgs.ai_skill_cardask["@zhensha"] = function(self, data)
	local carduse = data:toCardUse()
	if self:isFriend(carduse.from) then return "." end
	local cards = self.player:getHandcards()
	cards = sgs.QList2Table(cards)
	for _, fcard in ipairs(cards) do
		if fcard:getSuit() == sgs.Card_Spade then
			if carduse.from:isLord() or carduse.from:getLostHp() > 1 then
				return fcard:getEffectiveId()
			end
		end
	end
	return "."
end

function SmartAI:isNoZhenshaMark()
	if not self.player:isWounded() then return true end
	for _, enemy in ipairs(self:getEnemies(self.player)) do
		if not enemy:isKongcheng() and enemy:getMark("@methanol") > 0 then return false end
	end
	return true
end

-- lishishi
-- qinxin
sgs.ai_skill_invoke["qinxin"] = sgs.ai_skill_invoke["shalu"]

-- yinjian
sgs.ai_card_intention.YinjianCard = -75

local yinjian_skill={}
yinjian_skill.name = "yinjian"
table.insert(sgs.ai_skills, yinjian_skill)
yinjian_skill.getTurnUseCard = function(self)
	if self.player:hasUsed("YinjianCard") or self.player:getHandcardNum() <= 2 then return end
	return sgs.Card_Parse("@YinjianCard=.")
end
sgs.ai_skill_use_func["YinjianCard"] = function(card, use, self)
	local from, to
	for _, friend in ipairs(self.friends_noself) do
		if friend:isMale() then
			from = friend
			break
		end
	end
	if from then
		for _, friend in ipairs(self.friends) do
			if friend:isMale() and friend ~= from
				and friend:getKingdom() ~= from:getKingdom() then
				to = friend
				break
			end
		end
		if to then
			local cards = sgs.QList2Table(self.player:getCards("h"))
			self:sortByUseValue(cards, true)
			local yinjiancards = {}
			table.insert(yinjiancards, cards[1]:getEffectiveId())
			table.insert(yinjiancards, cards[2]:getEffectiveId())
			use.card = sgs.Card_Parse("@YinjianCard=" .. table.concat(yinjiancards, "+"))
			if use.to then
				self:speak("yinjian")
				use.to:append(from)
				use.to:append(to)
			end
			return
		end
	end
end

-- yanxijiao
-- suocai
sgs.ai_card_intention.SuocaiCard = 65

local suocai_skill={}
suocai_skill.name = "suocai"
table.insert(sgs.ai_skills, suocai_skill)
suocai_skill.getTurnUseCard = function(self)
	if not self.player:hasUsed("SuocaiCard") and not self.player:isKongcheng() then
		local max_card = self:getMaxCard()
		if max_card and self.player:getHandcardNum() > 2 then
			return sgs.Card_Parse("@SuocaiCard=" .. max_card:getEffectiveId())
		end
	end
end
sgs.ai_skill_use_func["SuocaiCard"]=function(card,use,self)
	self:sort(self.enemies, "handcard")
	for _, enemy in ipairs(self.enemies) do
		if not enemy:isKongcheng() and enemy:isMale() then
			use.card = card
			if use.to then
				self:speak("suocai")
				use.to:append(enemy)
			end
			return
		end
	end
end

-- huakui
sgs.ai_skill_invoke["huakui"] = function(self, data)
	self:speak("huakui")
	return true
end

-- wusheng
sgs.ai_view_as.wusheng = function(card, player, card_place)
	local suit = card:getSuitString()
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()
	if card:isRed() then
		return ("slash:wusheng[%s:%s]=%d"):format(suit, number, card_id)
	end
end

local wusheng_skill={}
wusheng_skill.name="wusheng"
table.insert(sgs.ai_skills,wusheng_skill)
wusheng_skill.getTurnUseCard=function(self,inclusive)
	local cards = self.player:getCards("he")
	cards=sgs.QList2Table(cards)
	local red_card
	self:sortByUseValue(cards,true)
	for _,card in ipairs(cards) do
		if card:isRed() then
			red_card = card
			break
		end
	end
	if red_card then
		local suit = red_card:getSuitString()
		local number = red_card:getNumberString()
		local card_id = red_card:getEffectiveId()
		local card_str = ("slash:wusheng[%s:%s]=%d"):format(suit, number, card_id)
		local slash = sgs.Card_Parse(card_str)
		assert(slash)
		return slash
	end
end
