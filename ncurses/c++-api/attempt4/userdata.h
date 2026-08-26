
#pragma once



class UserData {
public:
	explicit UserData(int value);
	[[nodiscard]] int value() const { return m_value; }
private:
	int m_value;
};


