#ifndef EXERCISE_H
#define EXERCISE_H

#include <string>
#include <glibmm/ustring.h>

class Exercise {
public:
    Exercise(int id = 0, const Glib::ustring& name = "",
             const Glib::ustring& image_path = "",
             const Glib::ustring& category = "",
             const Glib::ustring& primary_muscle = "",
             const Glib::ustring& secondary_muscles = "",
             const Glib::ustring& equipment = "",
             const Glib::ustring& difficulty_level = "",
             const Glib::ustring& instructions = "",
             const Glib::ustring& tips = "",
             const Glib::ustring& video_url = "",
             const Glib::ustring& reps_sets_suggestion = "",
             const Glib::ustring& benefits = "",
             const Glib::ustring& common_mistakes = "",
             const Glib::ustring& variations = "",
             int calories_burned_estimate = 0, // Changed to int
             const Glib::ustring& updated_at = "")
        : id(id), name(name), image_path(image_path), category(category),
          primary_muscle(primary_muscle), secondary_muscles(secondary_muscles),
          equipment(equipment), difficulty_level(difficulty_level),
          instructions(instructions), tips(tips), video_url(video_url),
          reps_sets_suggestion(reps_sets_suggestion), benefits(benefits),
          common_mistakes(common_mistakes), variations(variations),
          calories_burned_estimate(calories_burned_estimate), // Changed
          updated_at(updated_at) {}

    int get_id() const { return id; }
    Glib::ustring get_name() const { return name; }
    Glib::ustring get_image_path() const { return image_path; }
    Glib::ustring get_category() const { return category; }
    Glib::ustring get_primary_muscle() const { return primary_muscle; }
    Glib::ustring get_secondary_muscles() const { return secondary_muscles; }
    Glib::ustring get_equipment() const { return equipment; }
    Glib::ustring get_difficulty_level() const { return difficulty_level; }
    Glib::ustring get_instructions() const { return instructions; }
    Glib::ustring get_tips() const { return tips; }
    Glib::ustring get_video_url() const { return video_url; }
    Glib::ustring get_reps_sets_suggestion() const { return reps_sets_suggestion; }
    Glib::ustring get_benefits() const { return benefits; }
    Glib::ustring get_common_mistakes() const { return common_mistakes; }
    Glib::ustring get_variations() const { return variations; }
    int get_calories_burned_estimate() const { return calories_burned_estimate; } // Changed to int
    Glib::ustring get_updated_at() const { return updated_at; }

private:
    int id;
    Glib::ustring name;
    Glib::ustring image_path;
    Glib::ustring category;
    Glib::ustring primary_muscle;
    Glib::ustring secondary_muscles;
    Glib::ustring equipment;
    Glib::ustring difficulty_level;
    Glib::ustring instructions;
    Glib::ustring tips;
    Glib::ustring video_url;
    Glib::ustring reps_sets_suggestion;
    Glib::ustring benefits;
    Glib::ustring common_mistakes;
    Glib::ustring variations;
    int calories_burned_estimate; // Changed to int
    Glib::ustring updated_at;
};

#endif // EXERCISE_H
