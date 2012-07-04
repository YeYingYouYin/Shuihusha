-- ai for gift-package

function SmartAI:useCardZongzi(card, use)
	if self.player:hasSkill("lisao") then return end
	self:speak("gift")
	use.card = card
end

sgs.ai_use_value.Zongzi = 5.5
sgs.ai_keep_value.Zongzi = 0
sgs.dynamic_value.benefit.Zongzi = true
sgs.ai_card_intention.Zongzi = function(card,from,tos)
	speakTrigger(card,from,tos[1])
	sgs.updateIntentions(from, tos, 0)
end