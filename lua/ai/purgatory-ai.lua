-- ai for purgatory-package

sgs.ai_use_value.Mastermind = 3
sgs.ai_keep_value.Mastermind = 0
sgs.ai_use_priority.Mastermind = 3

function SmartAI:useCardMastermind(card, use)
	local en, fr
	self:sort(self.enemies, "defense")
	if #self.enemies > 0 then
		en = self.enemies[1]
	end
	self:sort(self.friends, "hp")
	if #self.friends > 0 then
		fr = self.friends[1]
	end
	if en and fr then
		use.card = card
		if use.to then
			use.to:append(fr)
			use.to:append(en)
		end
		return
	end
end

sgs.ai_use_value.SpinDestiny = 3
sgs.ai_keep_value.SpinDestiny = 0
sgs.ai_use_priority.SpinDestiny = 3

function SmartAI:useCardSpinDestiny(card, use)
	if #self.enemies > #self.friends_noself then
		use.card = card
	end
end

sgs.ai_use_value.EdoTensei = 3
sgs.ai_keep_value.EdoTensei = 0

function SmartAI:useCardShit(card, use)
	if self.player:hasSkill("fushang") and self.player:getMaxHP() > 3 then return end
	if (card:getSuit() == sgs.Card_Heart or card:getSuit() == sgs.Card_Club) and self.player:isChained() and
		#(self:getChainedFriends()) > #(self:getChainedEnemies()) then return end
	if self.player:getHp()>3 and (self.player:hasSkill("huanshu") or self.player:hasSkill("shenchou")) then use.card = card return end
	if card:getSuit() == sgs.Card_Heart and (self:isEquip("GaleShell") or self:isEquip("Vine")) then return end
	if not self.player:isWounded() then
		if self:hasSkills(sgs.need_kongcheng) and self.player:getHandcardNum() == 1 then
			use.card = card
			return
		end
		if sgs[self.player:getGeneralName() .. "_suit_value"] and
			(sgs[self.player:getGeneralName() .. "_suit_value"][card:getSuitString()] or 0) > 0 then return end
		local peach = self:getCard("Peach")
		if peach then
			self:sort(self.friends, "hp")
			if not self:isWeak(self.friends[1]) then
				use.card = card
				return
			end
		end
	end
end

sgs.ai_use_value.Shit = -10
sgs.ai_keep_value.Shit = 6

sgs.weapon_range.LashGun = 6
function sgs.ai_armor_value.proud_banner(player, self)
	return 3.5
end
function sgs.ai_trick_prohibit.proud_banner(card, self, to)
	return to:hasEquip("proud_banner") and not to:isChained() and card:isKindOf("IronChain")
end
