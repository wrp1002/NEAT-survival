#include "Food.h"

#include "Globals.h"

void Food::ConvertToFood() {
	objectType = "food";
	SetColor(al_map_rgb(0, 255, 0));
	foodType = FOOD;
}

void Food::ConvertToWaste() {
	objectType = "waste";
	SetColor(al_map_rgb(168, 111, 50));
	foodType = WASTE;
}

void Food::ConvertToMeat() {
	objectType = "meat";
	SetColor(al_map_rgb(255, 0, 0));
	foodType = MEAT;
}

Food::Food(Vector2f startingPos, double startingEnergy, int newType) : Object(startingPos.x, startingPos.y, GetRadiusForEnergy(startingEnergy)) {
	energy = startingEnergy;
	foodType = newType;
	life = 1000 + rand() % 500;

	pos.x += Globals::Random() * 5 - 2.5;
	pos.y += Globals::Random() * 5 - 2.5;

	float randVel = 20;
	vel.x += Globals::Random() * randVel - randVel / 2;
	vel.y += Globals::Random() * randVel - randVel / 2;

	if (foodType == FOOD)
		ConvertToFood();
	if (foodType == WASTE)
		ConvertToWaste();
	else if (foodType == MEAT)
		ConvertToMeat();

	//cout << "New food" << pos.x << pos.y << " " << GetType() << endl;
}

void Food::Update() {
	if (life > 0) {
		if (rand() % 2 == 0)
			life--;
		if (life == 0) {
			if (IsWaste())
				ConvertToFood();
		}
	}
	if (energy <= 0)
		alive = false;


	// Move towards other food to merge
	if (energy < MAX_ENERGY) {
		vector<shared_ptr<Object>> nearbyFood = GetNearbyObjects(1, "food");
		shared_ptr<Object> targetFood = nearbyFood.size() ? nearbyFood[rand() % nearbyFood.size()] : nullptr;

		if (targetFood && targetFood.get() != this) {
			float dir = GetAngleTo(targetFood);
			float dist = pos.GetDistance(targetFood->GetPos());
			float force = dist ? (1.0 / (dist)) : 0;
			vel += Vector2f(cos(dir) * force, -sin(dir) * force);
		}
	}

	Object::Update();
}

void Food::CollisionEvent(shared_ptr<Object> other) {
	if (shared_ptr<Food> otherFood = dynamic_pointer_cast<Food>(other)) {
		if (!otherFood->IsAlive() || !IsAlive())
			return;

		if (IsWaste() || otherFood->IsWaste())
			return;

		if (GetEnergy() + otherFood->GetEnergy() > MAX_ENERGY)
			return;

		SetEnergy(GetEnergy() + otherFood->GetEnergy());
		otherFood->SetEnergy(0);
		otherFood->SetAlive(false);
		SetRadius(GetRadiusForEnergy(energy));
	}
}

void Food::Draw() {
	al_draw_filled_circle(pos.x, pos.y, radius, color);
}

double Food::GetEnergy() {
	return energy;
}

float Food::GetRadiusForEnergy(double energy) {
	return (energy / MAX_ENERGY) * MAX_RADIUS;
}

bool Food::IsFood() {
	return foodType == FOOD;
}

bool Food::IsWaste() {
	return foodType == WASTE;
}

bool Food::IsMeat() {
	return foodType == MEAT;
}

void Food::SetEnergy(double newEnergy) {
	energy = newEnergy;
	if (energy <= 0)
		alive = false;
}
