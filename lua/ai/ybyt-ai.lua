-- hunjiu
hunjiu_skill={}
hunjiu_skill.name = "hunjiu"
table.insert(sgs.ai_skills, hunjiu_skill)
hunjiu_skill.getTurnUseCard = function(self)
	local cards = self.player:getCards("h")
	cards=sgs.QList2Table(cards)
	local card
	self:sortByUseValue(cards,true)
	for _,acard in ipairs(cards)  do
		if (acard:inherits("Ecstasy") or acard:inherits("Analeptic") or
			(not self.player:isWounded() and acard:inherits("Peach"))) then
			card = acard
			break
		end
	end
	if not card then return nil end
	local suit = card:getSuitString()
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()
	local card_str = ("analeptic:hunjiu[%s:%s]=%d"):format(suit, number, card_id)
	if card:inherits("Analeptic") then
		card_str = ("ecstasy:hunjiu[%s:%s]=%d"):format(suit, number, card_id)
	end
	local caard = sgs.Card_Parse(card_str)
	assert(caard)
	return caard
end
sgs.ai_view_as["hunjiu"] = function(card, player, card_place, class_name)
	local suit = card:getSuitString()
	local number = card:getNumberString()
	local card_id = card:getEffectiveId()

	if card:inherits("Peach") or card:inherits("Analeptic") or card:inherits("Ecstasy") then
		if class_name == "Analeptic" then
			return ("analeptic:hunjiu[%s:%s]=%d"):format(suit, number, card_id)
		else
			return ("ecstasy:hunjiu[%s:%s]=%d"):format(suit, number, card_id)
		end
	end
end

-- goulian
sgs.ai_skill_invoke["goulian"] = sgs.ai_skill_invoke["liba"]

-- sinue
sgs.ai_skill_use["@@sinue"] = function(self, prompt)
	local cards = self.player:getHandcards()
	cards = sgs.QList2Table(cards)
	self:sortByUseValue(cards, true)
	for _, enemy in ipairs(self.enemies) do
		if self.player:distanceTo(enemy) == 1 then
			return "@SinueCard=" .. cards[1]:getEffectiveId()
		end
	end
	return "."
end

-- shexin
local shexin_skill={}
shexin_skill.name = "shexin"
table.insert(sgs.ai_skills, shexin_skill)
shexin_skill.getTurnUseCard = function(self)
    if not self.player:hasUsed("ShexinCard") and not self.player:isNude() then
		self:sort(self.enemies, "handcard")
		if self.enemies[#self.enemies]:getHandcardNum() <= 3 then return end
		local cards = self.player:getCards("he")
		cards = sgs.QList2Table(cards)
		self:sortByUseValue(cards, true)
		for _, card in ipairs(cards) do
			if card:isNDTrick() or card:inherits("EquipCard") then
				return sgs.Card_Parse("@ShexinCard=" .. card:getEffectiveId())
			end
		end
	end
end
sgs.ai_skill_use_func["ShexinCard"]=function(card,use,self)
	self:sort(self.enemies, "handcard")
	if use.to then
		use.to:append(self.enemies[#self.enemies])
	end
    use.card=card
end

-- jiayao
sgs.ai_skill_invoke["jiayao"] = true
