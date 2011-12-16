-- haoshen
sgs.ai_skill_use["@@haoshen"] = function(self, prompt)
	if prompt == "@haoshen-draw" then
		self:sort(self.friends_noself)
		local max_x = 2
		local target
		for _, friend in ipairs(self.friends_noself) do
			local x = friend:getMaxHP() - friend:getHandcardNum()
			if x > max_x then
				max_x = x
				target = friend
			end
		end
		if target then
			return "@HaoshenCard=.->" .. target:objectName()
		else
			return "."
		end
	elseif prompt == "@haoshen-play" and self.player:getHandcardNum() > 5 then
		self:sort(self.friends_noself, "hp")
		local target = self.friends_noself[1]
		local cards = self.player:getHandcards()
		cards = sgs.QList2Table(cards)
		self:sortByUseValue(cards, true)
		local card_ids = {}
		for i = 1, math.floor(#cards / 2) do
			table.insert(card_ids, cards[i]:getEffectiveId())
		end
		return "@HaoshenCard=" .. table.concat(card_ids, "+") .. "->" .. target:objectName()
	else
		return "."
	end
end

-- huxiao
local huxiao_skill={}
huxiao_skill.name = "huxiao"
table.insert(sgs.ai_skills, huxiao_skill)
huxiao_skill.getTurnUseCard = function(self)
	if not self.player:isNude() then
		local cards = self.player:getCards("he")
		cards = sgs.QList2Table(cards)
		for _, fcard in ipairs(cards) do
			if fcard:inherits("EquipCard") then
				local suit, number, id = fcard:getSuitString(), fcard:getNumberString(), fcard:getId()
				local card_str = ("savage_assault:huxiao[%s:%s]=%d"):format(suit, number, id)
				local savage = sgs.Card_Parse(card_str)
				assert(savage)
				return savage
			end
		end
	end
end

-- baoguo&yinyu
sgs.ai_skill_invoke["baoguo"] = true
sgs.ai_skill_invoke["yinyu"] = true

-- taohui
sgs.ai_skill_playerchosen["taohui"] = function(self, targets)
	self:sort(self.friends, "handcard")
	return self.friends[1]
end

-- huanshu
sgs.ai_skill_use["@@huanshu"] = function(self, prompt)
	self:sort(self.enemies, "hp")
	local target = self.enemies[1]
	if target then return "@HuanshuCard=.".."->"..target:objectName() end
	return "."
end

-- cuju
sgs.ai_skill_invoke["cuju"] = true
sgs.ai_skill_use["@@cuju"] = function(self, prompt)
	self:sort(self.enemies, "hp")
	local target = self.enemies[1]
	local card = self.player:getRandomHandCard()
	if target then return "@CujuCard="..card:getEffectiveId().."->"..target:objectName() end
	return "."
end

-- panquan
sgs.ai_skill_invoke["panquan"] = function(self, data)
	local gaoqiu = self.room:getLord()
	return self:isFriend(gaoqiu)
end


duanliang_skill={}
duanliang_skill.name="duanliang"
table.insert(sgs.ai_skills,duanliang_skill)
duanliang_skill.getTurnUseCard=function(self)
	local cards = self.player:getCards("he")
	cards=sgs.QList2Table(cards)

	local card

	self:sortByUseValue(cards,true)

	for _,acard in ipairs(cards)  do
		if (acard:isBlack()) and (acard:inherits("BasicCard") or acard:inherits("EquipCard")) then
			card = acard
			break
		end
	end

		if not card then return nil end
		local suit = card:getSuitString()
		local number = card:getNumberString()
		local card_id = card:getEffectiveId()
		local card_str = ("supply_shortage:duanliang[%s:%s]=%d"):format(suit, number, card_id)
		local skillcard = sgs.Card_Parse(card_str)

		assert(skillcard)

		return skillcard

end

dimeng_skill={}
dimeng_skill.name="dimeng"
table.insert(sgs.ai_skills,dimeng_skill)
dimeng_skill.getTurnUseCard=function(self)
	if self.player:hasUsed("DimengCard") then return nil end
	card=sgs.Card_Parse("@DimengCard=.")
	return card

end

sgs.ai_skill_use_func["DimengCard"]=function(card,use,self)
	local cardNum=self.player:getHandcardNum()

	self:sort(self.enemies,"handcard")
	self:sort(self.friends_noself,"handcard")

	local lowest_friend=self.friends_noself[1]

	self:sort(self.enemies,"defense")
	if lowest_friend then
		for _,enemy in ipairs(self.enemies) do
			local hand1=enemy:getHandcardNum()
			local hand2=lowest_friend:getHandcardNum()

			if (hand1 > hand2) then
				if (hand1-hand2)<=cardNum then
					use.card=card
					if use.to then
						use.to:append(enemy)
						use.to:append(lowest_friend)
						return
					end
				end
			end
		end
	end
end

luanwu_skill={}
luanwu_skill.name="luanwu"
table.insert(sgs.ai_skills, luanwu_skill)
luanwu_skill.getTurnUseCard=function(self)
	if self.player:getMark("@chaos") <= 0 then return end
	local good, bad = 0, 0
	for _, player in sgs.qlist(self.room:getOtherPlayers(self.player)) do
		if self:isWeak(player) then
			if self:isFriend(player) then bad = bad + 1
			else good = good + 1
			end
		end
	end
	if good == 0 then return end

	for _, player in sgs.qlist(self.room:getOtherPlayers(self.player)) do
		if self:getCardsNum("Analeptic", player) > 0 then
			if self:isFriend(player) then good = good + 1.0/player:getHp()
			else bad = bad + 1.0/player:getHp()
			end
		end

		local has_slash = self:getCard("Slash", player)
		local can_slash = false
		if not can_slash then
			for _, p in sgs.qlist(self.room:getOtherPlayers(player)) do
				if player:inMyAttackRange(p) then can_slash = true break end
			end
		end
		if not has_slash or not can_slash then
			if self:isFriend(player) then good = good + math.max(self:getCardsNum("Peach", player), 1)
			else bad = bad + math.max(self:getCardsNum("Peach", player), 1)
			end
		end

		if self:getCardsNum("Jink", player) == 0 then
			local lost_value = 0
			if self:hasSkills(sgs.masochism_skill, player) then lost_value = player:getHp()/2 end
			if self:isFriend(player) then bad = bad + (lost_value+1)/player:getHp()
			else good = good + (lost_value+1)/player:getHp()
			end
		end
	end

	if good > bad then return sgs.Card_Parse("@LuanwuCard=.") end
end

sgs.ai_skill_use_func["LuanwuCard"]=function(card,use,self)
	use.card = card
end

sgs.ai_skill_playerchosen.luanwu = sgs.ai_skill_playerchosen.zero_card_as_slash
