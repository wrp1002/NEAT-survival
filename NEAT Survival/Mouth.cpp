#include "Mouth.h"
#include "Agent.h"

Mouth::Mouth(shared_ptr<Agent> parent, float radius) {
	this->parentPtr = parent;
	this->radius = radius;
	this->deltaPos = parent->GetRadius();
	this->pos = parent->GetPos();
	this->dir = parent->GetDir();
	cooldownTimerStart = 60;
	cooldownTimer = 0;
	wantsToBite = false;
}

void Mouth::UpdatePosition() {
	shared_ptr<Agent> parent = parentPtr.lock();
	this->dir = parent->GetDir();
	this->pos = parent->GetPos() + Vector2f::FromDir(dir, parent->GetRadius());
}

void Mouth::Update(vector<shared_ptr<Object>> nearbyObjects) {
	if (parentPtr.expired()) {
		cout << "mouth tried to update with null parent" << endl;
		return;
	}
	
	collidingObjectPtr.reset();
	for (auto obj : nearbyObjects) {
		if (CollidesWith(obj) && obj != parentPtr.lock()) {
			collidingObjectPtr = obj;
			break;
		}
	}

	if (cooldownTimer > 0) {
		cooldownTimer--;
	}
}

void Mouth::Draw() {
	ALLEGRO_COLOR color = al_map_rgb(100, 0, 0);
	if (CanBite()) {
		if (wantsToBite)
			color = al_map_rgb(255, 0, 0);

		Vector2f pos1 = pos + Vector2f::FromDir(dir, radius);
		Vector2f pos2 = pos + Vector2f::FromDir(dir + M_PI_2, radius / 2);
		Vector2f pos3 = pos + Vector2f::FromDir(dir - M_PI_2, radius / 2);
		al_draw_filled_triangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, color);
	}
	else {
		al_draw_filled_circle(pos.x, pos.y, radius / 2, color);
	}
}

bool Mouth::CanBite() {
	return cooldownTimer == 0;
}

void Mouth::Bite() {
	if (collidingObjectPtr.expired() || parentPtr.expired() || !collidingObjectPtr.lock()->IsAlive())
		return;

	shared_ptr<Agent> parent = parentPtr.lock();

	if (shared_ptr<Food> food = dynamic_pointer_cast<Food>(collidingObjectPtr.lock())) {
		if (food->IsFood()) {
			double eatAmount = Globals::Constrain((double)parent->GetDamage(), 0.0, food->GetEnergy());
			parent->AddEnergy(eatAmount);
			food->SetEnergy(food->GetEnergy() - eatAmount);
		}
		else if (food->IsWaste()) {
			if (!GameRules::IsRuleEnabled("WasteDamage"))
				return;

			double eatAmount = Globals::Constrain((double)parent->GetDamage(), 0.0, food->GetEnergy());
			//double usableEnergy = eatAmount * 0.1;
			double waste = eatAmount;
			double healthLoss = eatAmount / 2;

			//parent->AddEnergy(usableEnergy);
			parent->AddWaste(waste);
			parent->HealthToWaste(healthLoss);

			food->SetEnergy(food->GetEnergy() - waste);
		}
	}
	else if (shared_ptr<Agent> agent = dynamic_pointer_cast<Agent>(collidingObjectPtr.lock())) {
		double eatAmount = Globals::Constrain((double)parent->GetDamage(), 0.0, agent->GetHealth());
		agent->SetHealth(agent->GetHealth() - eatAmount);
		parent->AddEnergy(eatAmount);
	}

	cooldownTimer = cooldownTimerStart;
}

void Mouth::SetWantsToBite(bool val) {
	wantsToBite = val;
}

bool Mouth::ObjectInMouth() {
	return !collidingObjectPtr.expired();
}

weak_ptr<Object> Mouth::GetObjectInMouth() {
	return collidingObjectPtr;
}
