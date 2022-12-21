#include "Application.h"
#include "imgui.h"

#include <string>
#include <iostream>
#include <vector>

namespace GSLCalc
{

    static const char* total_holes[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
    static const char* score_text_par_three[] = { "Hole In One", "Birdie", "Par",
                                                "Bogey", "Double Bogey", "Triple Bogey",
                                                "+4", "+5", "+6" };
    static const char* score_text_par_four[] = { "Hole In One", "Eagle", "Birdie", "Par",
                                                "Bogey", "Double Bogey", "Triple Bogey",
                                                "+4", "+5", "+6" , "+7", "+8"};
    static const char* score_text_par_five[] = { "Hole In One", "Albatross", "Eagle", "Birdie", "Par",
                                                "Bogey", "Double Bogey", "Triple Bogey",
                                                "+4", "+5", "+6", "+7", "+8", "+9", "+10"};
    static int hole_pars[] = { 4,3,5,3,5,4,4,3,5 };

    static int par_four_possible_values[] = {1,2,3,4,5,5,6,7,8,9,10,11,12};
    static int par_three_possible_values[] = {1,2,3,4,5,6,7,8,9};
    static int par_five_possible_values[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

    static ImGuiComboFlags flags[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    static int item_current_idx[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    const char* combo_preview_value[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    static int score_value[] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
    int target_skill_level = 0;
    int target_games = 1;
    float current_skill_level;
    float difficulty_bonus;
    float skill_level = 0.0;
    float par_differential;
    bool simulation_possible = true;
    std::vector<int> hole_combo;

    static float CalculateSkillLevel(float current_skill_level, int shots_taken, float difficulty_bonus);
    static int current_course;
    static void PrintNewSkill(double skill_level);
    static std::pair<std::vector<int>, bool> SimulateSkillLevelCalc(int current_course, float sim_current_skill_level, int target_skill_level);

    void RenderUI()
    {
        //Begin The Render
        ImGui::Begin("Calculator");
        //ImGui::SetWindowFontScale(2);
        ImGui::GetIO().FontGlobalScale = 2;

        //Select Course
        if (ImGui::Button("Beginner"))
        {
            current_course = 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("Intermediate"))
        {
            current_course = 2;
        }
        ImGui::SameLine();
        if (ImGui::Button("Expert"))
        {
            current_course = 3;
        }
        ImGui::SameLine();
        if (ImGui::Button("9 Hole"))
        {
            current_course = 4;
        }

        //Select Current Skill Level
        static float f0 = current_skill_level;
        ImGui::Text("Current Skill Level");
        ImGui::SameLine();
        ImGui::InputFloat(" ", &current_skill_level, 0.01f, 1.0f, "%.2f");
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            current_skill_level = 0;
        }

        //Pick The Correct Holes
        switch (current_course)
        {
            default:
                //Draw Hole Selections

                //Hole 1
                combo_preview_value[0] = score_text_par_four[item_current_idx[0]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 1");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t", combo_preview_value[0], flags[0]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_four); n++)
                    {
                        const bool is_selected = (item_current_idx[0] == n);
                        if (ImGui::Selectable(score_text_par_four[n], is_selected))
                        {
                            item_current_idx[0] = n;
                            score_value[0] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                
                //Hole 2
                combo_preview_value[1] = score_text_par_three[item_current_idx[1]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 2");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t", combo_preview_value[1], flags[1]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_three); n++)
                    {
                        const bool is_selected = (item_current_idx[1] == n);
                        if (ImGui::Selectable(score_text_par_three[n], is_selected))
                        {
                            item_current_idx[1] = n;
                            score_value[1] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 3
                combo_preview_value[2] = score_text_par_five[item_current_idx[2]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 3");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t\t", combo_preview_value[2], flags[2]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_five); n++)
                    {
                        const bool is_selected = (item_current_idx[2] == n);
                        if (ImGui::Selectable(score_text_par_five[n], is_selected))
                        {
                            item_current_idx[2] = n;
                            score_value[2] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Button("Generate"))
                {
                    for (int n = 0; n < 3; n++)
                    {
                        difficulty_bonus = n / 8.0;
                        par_differential = score_value[n] - hole_pars[n];
                        skill_level = CalculateSkillLevel(current_skill_level, par_differential, difficulty_bonus);
                        current_skill_level = skill_level;
                    }
                }
                break;

            case 2:
                //Draw Hole Selections

                //Hole 4
                combo_preview_value[3] = score_text_par_three[item_current_idx[3]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 4");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t", combo_preview_value[3], flags[3]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_three); n++)
                    {
                        const bool is_selected = (item_current_idx[3] == n);
                        if (ImGui::Selectable(score_text_par_three[n], is_selected))
                        {
                            item_current_idx[3] = n;
                            score_value[3] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 5
                combo_preview_value[4] = score_text_par_five[item_current_idx[4]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 5");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t", combo_preview_value[4], flags[4]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_five); n++)
                    {
                        const bool is_selected = (item_current_idx[4] == n);
                        if (ImGui::Selectable(score_text_par_five[n], is_selected))
                        {
                            item_current_idx[4] = n;
                            score_value[4] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 6
                combo_preview_value[5] = score_text_par_four[item_current_idx[5]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 6");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t\t", combo_preview_value[5], flags[5]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_four); n++)
                    {
                        const bool is_selected = (item_current_idx[5] == n);
                        if (ImGui::Selectable(score_text_par_four[n], is_selected))
                        {
                            item_current_idx[5] = n;
                            score_value[5] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Button("Generate"))
                {
                    for (int n = 3; n < 6; n++)
                    {
                        difficulty_bonus = n / 8.0;
                        par_differential = score_value[n] - hole_pars[n];
                        skill_level = CalculateSkillLevel(current_skill_level, par_differential, difficulty_bonus);
                        current_skill_level = skill_level;
                    }
                }
                break;

            case 3:
                //Draw Hole Selections

                //Hole 7
                combo_preview_value[6] = score_text_par_four[item_current_idx[6]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 7");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t", combo_preview_value[6], flags[6]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_four); n++)
                    {
                        const bool is_selected = (item_current_idx[6] == n);
                        if (ImGui::Selectable(score_text_par_four[n], is_selected))
                        {
                            item_current_idx[6] = n;
                            score_value[6] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 8
                combo_preview_value[7] = score_text_par_three[item_current_idx[7]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 8");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t", combo_preview_value[7], flags[7]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_three); n++)
                    {
                        const bool is_selected = (item_current_idx[7] == n);
                        if (ImGui::Selectable(score_text_par_three[n], is_selected))
                        {
                            item_current_idx[7] = n;
                            score_value[7] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 9
                combo_preview_value[8] = score_text_par_five[item_current_idx[8]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 9");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t\t", combo_preview_value[8], flags[8]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_five); n++)
                    {
                        const bool is_selected = (item_current_idx[8] == n);
                        if (ImGui::Selectable(score_text_par_five[n], is_selected))
                        {
                            item_current_idx[8] = n;
                            score_value[8] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Button("Generate"))
                {
                    for (int n = 6; n < 9; n++)
                    {
                        difficulty_bonus = n / 8.0;
                        par_differential = score_value[n] - hole_pars[n];
                        skill_level = CalculateSkillLevel(current_skill_level, par_differential, difficulty_bonus);
                        current_skill_level = skill_level;
                    }
                }
                break;

            case 4:
                //Draw Hole Selections

                //Hole 1
                combo_preview_value[9] = score_text_par_four[item_current_idx[9]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 1");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t", combo_preview_value[9], flags[9]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_four); n++)
                    {
                        const bool is_selected = (item_current_idx[9] == n);
                        if (ImGui::Selectable(score_text_par_four[n], is_selected))
                        {
                            item_current_idx[9] = n;
                            score_value[9] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 2
                combo_preview_value[10] = score_text_par_three[item_current_idx[10]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 2");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t", combo_preview_value[10], flags[10]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_three); n++)
                    {
                        const bool is_selected = (item_current_idx[10] == n);
                        if (ImGui::Selectable(score_text_par_three[n], is_selected))
                        {
                            item_current_idx[10] = n;
                            score_value[10] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 3
                combo_preview_value[11] = score_text_par_five[item_current_idx[11]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 3");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t\t", combo_preview_value[11], flags[11]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_five); n++)
                    {
                        const bool is_selected = (item_current_idx[11] == n);
                        if (ImGui::Selectable(score_text_par_five[n], is_selected))
                        {
                            item_current_idx[11] = n;
                            score_value[11] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 4
                combo_preview_value[12] = score_text_par_three[item_current_idx[12]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 4");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t\t\t", combo_preview_value[12], flags[12]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_three); n++)
                    {
                        const bool is_selected = (item_current_idx[12] == n);
                        if (ImGui::Selectable(score_text_par_three[n], is_selected))
                        {
                            item_current_idx[12] = n;
                            score_value[12] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 5
                combo_preview_value[13] = score_text_par_five[item_current_idx[13]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 5");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t\t\t\t", combo_preview_value[13], flags[13]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_five); n++)
                    {
                        const bool is_selected = (item_current_idx[13] == n);
                        if (ImGui::Selectable(score_text_par_five[n], is_selected))
                        {
                            item_current_idx[13] = n;
                            score_value[13] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 6
                combo_preview_value[14] = score_text_par_four[item_current_idx[14]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 6");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t\t\t\t\t", combo_preview_value[14], flags[14]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_four); n++)
                    {
                        const bool is_selected = (item_current_idx[14] == n);
                        if (ImGui::Selectable(score_text_par_four[n], is_selected))
                        {
                            item_current_idx[14] = n;
                            score_value[14] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 7
                combo_preview_value[15] = score_text_par_four[item_current_idx[15]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 7");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t\t\t\t\t\t", combo_preview_value[15], flags[15]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_four); n++)
                    {
                        const bool is_selected = (item_current_idx[15] == n);
                        if (ImGui::Selectable(score_text_par_four[n], is_selected))
                        {
                            item_current_idx[15] = n;
                            score_value[15] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 8
                combo_preview_value[16] = score_text_par_three[item_current_idx[16]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 8");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t\t\t\t\t\t\t", combo_preview_value[16], flags[16]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_three); n++)
                    {
                        const bool is_selected = (item_current_idx[16] == n);
                        if (ImGui::Selectable(score_text_par_three[n], is_selected))
                        {
                            item_current_idx[16] = n;
                            score_value[16] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                //Hole 9
                combo_preview_value[17] = score_text_par_five[item_current_idx[17]];  // Pass in the preview value visible before opening the combo (it could be anything)
                ImGui::Text("Hole 9");
                ImGui::SameLine();
                if (ImGui::BeginCombo("\t\t\t\t\t\t\t\t\t", combo_preview_value[17], flags[17]))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(score_text_par_five); n++)
                    {
                        const bool is_selected = (item_current_idx[17] == n);
                        if (ImGui::Selectable(score_text_par_five[n], is_selected))
                        {
                            item_current_idx[17] = n;
                            score_value[17] = n + 1;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Button("Generate"))
                {
                    for (int n = 0; n < 9; n++)
                    {
                        difficulty_bonus = n / 8.0;
                        par_differential = score_value[n] - hole_pars[n];
                        skill_level = CalculateSkillLevel(current_skill_level, par_differential, difficulty_bonus);
                        current_skill_level = skill_level;
                    }
                }
                break;
                
        }

        //Print The New Skill
        PrintNewSkill(skill_level);

        //Get Target Skill Level
        ImGui::Text("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
        ImGui::Text("Target Skill Level");
        ImGui::SameLine();
        ImGui::InputInt("\t\t\t\t\t\t\t\t\t\t", &target_skill_level, 1);
        ImGui::SameLine();
        if (ImGui::Button("Pro"))
        {
            target_skill_level = 1000;
        }

        //Select Target Games
        ImGui::Text("Target Games");
        ImGui::SameLine();
        ImGui::InputInt("\t\t\t\t\t\t\t\t\t\t\t", &target_games, 1);

        
        //error text
        if (target_skill_level < current_skill_level)
        {
            target_skill_level = current_skill_level;
        }
        else if (target_skill_level > 3462)
        {
            target_skill_level = 3462;
        }
        if (target_games != 1)
        {
            target_games = 1;
        }

        //Calculate Skill Needed
        float skill_needed = target_skill_level - current_skill_level;
        float skill_needed_per_game = skill_needed / target_games;

        //Simulate Skill Level
        if (ImGui::Button("Simulate"))
        {
            std::pair<std::vector<int>, bool> simulate_result = SimulateSkillLevelCalc(current_course, current_skill_level, target_skill_level);
            std::vector<int> &hole_combo = simulate_result.first;
            simulation_possible = simulate_result.second;
        }
        if (!simulation_possible)
        {
            ImGui::Text("Target Skill Level Not Possible In One Game");
        }
        if (simulation_possible && !hole_combo.empty())
        {
            //Write Best Combination of Score
            ImGui::Text("Best Score Combination: ");
            ImGui::Text("Hole 1: ");
            ImGui::SameLine();

        }

        ImGui::End();
    }

    /*
    *   Skill Level Formula:
    *
    * [Skill level] ← [Skill level] x 0.97 + 30 x √ (2 ^ [score coefficient])
    * [Score coefficient] = [Relative score] × -1 + [Difficulty bonus]
    * [Difficulty bonus] = ([Hole number] -1) ÷ 8
    * 
    */
    static float CalculateSkillLevel(float current_skill_level, int par_differential, float difficulty_bonus)
    {
        //Calculate Score Coefficient
        float score_coefficient = -1.0 * par_differential + difficulty_bonus;

        //Calculate Skill Level
        return (current_skill_level * 0.97f) + (30.0f * sqrt(pow(2.0f, score_coefficient)));
    }

    static void PrintNewSkill(double skill_level)
    {
        std::string skill_level_string = std::to_string(skill_level);
        char const* skill_level_char = skill_level_string.c_str();
        ImGui::Text("New Skill Level: ");
        ImGui::SameLine();
        ImGui::Text(skill_level_char);
    }

    static std::pair<std::vector<int>, bool> SimulateSkillLevelCalc(int current_course, float sim_current_skill_level, int target_skill_level)
    {
        bool tsl_possible = true;
        int sim_combined_score;
        std::vector<int> best_hole_combination;
        switch (current_course)
        {
            default:
                //beginner calculations
                if (target_games == 1)
                {
                    for (int n = 6; n > 0; n--)
                    {
                        //Simulate Difficulty Bonus For Hole 1
                        float sim_difficulty_bonus = 0;

                        //Simualate Par Differential For Hole 1
                        int sim_par_differential = (n - 1) - 4;

                        //Simulate Score Coefficient For Hole 1
                        float sim_score_coefficient = -1.0 * sim_par_differential + sim_difficulty_bonus;

                        //Simulate Skill Level For Hole 1
                        float sim_skill_level = (sim_current_skill_level * 0.97f) + (30.0f * sqrt(pow(2.0f, sim_score_coefficient)));


                        //Simulate Difficulty Bonus For Hole 2
                        sim_difficulty_bonus = 1.0/8.0;

                        //Simualate Par Differential For Hole 2
                        sim_par_differential = (n - 1) - 3;

                        //Simulate Score Coefficient For Hole 2
                        sim_score_coefficient = -1.0 * sim_par_differential + sim_difficulty_bonus;

                        //Simulate Skill Level For Hole 2
                        sim_skill_level = (sim_current_skill_level * 0.97f) + (30.0f * sqrt(pow(2.0f, sim_score_coefficient)));


                        //Simulate Difficulty Bonus For Hole 3
                        sim_difficulty_bonus = 2.0/8.0;

                        //Simualate Par Differential For Hole 3
                        sim_par_differential = (n - 1) - 5;

                        //Simulate Score Coefficient For Hole 3
                        sim_score_coefficient = -1.0 * sim_par_differential + sim_difficulty_bonus;

                        //Simulate Skill Level For Hole 3
                        sim_skill_level = (sim_current_skill_level * 0.97f) + (30.0f * sqrt(pow(2.0f, sim_score_coefficient)));

                        if (sim_skill_level >= target_skill_level)
                        {
                            sim_combined_score = n;
                            tsl_possible = true;
                            break;
                        }
                        if (n = 1 && sim_skill_level < target_skill_level)
                        {
                            tsl_possible = false;
                            break;
                        }
                    }
                    if (tsl_possible)
                    {
                        best_hole_combination.push_back(sim_combined_score);
                        best_hole_combination.push_back(sim_combined_score);
                        best_hole_combination.push_back(sim_combined_score);
                    }
                }

                return { best_hole_combination, tsl_possible};
                break;

            case 2:
                //intermediate calculations
                break;

            case 3:
                //expert calculations
                break;

            case 4:
                //9 hole calculations
                break;
        }
    }
}
