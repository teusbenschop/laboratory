

#pragma once

#include <cursesapp.h>
#include <cursesf.h>
#include <cursesm.h>

class Application : public NCursesApplication {
  public:
	explicit Application(bool enable_colors);

	void handleArgs(int, char* []) override;

  protected:
	[[nodiscard]] int titlesize() const override;
	void title() override;
	[[nodiscard]] Soft_Label_Key_Set::Label_Layout useSLKs() const override;
	void init_labels(Soft_Label_Key_Set& soft_label_key_set) const override;
	int run() override;
};
