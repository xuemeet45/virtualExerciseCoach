-- Clear data from tables, except 'users'
TRUNCATE TABLE exercise_history RESTART IDENTITY CASCADE;
TRUNCATE TABLE user_sessions RESTART IDENTITY CASCADE;
-- TRUNCATE TABLE user_profiles RESTART IDENTITY CASCADE;
TRUNCATE TABLE exercises RESTART IDENTITY CASCADE;
-- TRUNCATE TABLE users RESTART IDENTITY CASCADE; -- Do not truncate users table as per user's request

-- Insert 10 easy front-facing yoga poses

-- 1. Mountain Pose (Tadasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('山のポーズ (ターダーサナ)', 'images/mountain_pose.jpeg', 'Yoga', '全身', '初級',
E'- 足を揃えて立ちます。\n- 腕を体の横か頭上に伸ばします。\n- 背筋を伸ばし、肩をリラックスさせます。\n- 呼吸に集中します。',
'足の裏全体で均等に体重を支え、体の軸を感じます。',
'姿勢の改善、バランス感覚の向上、心の落ち着き。',
'30秒間保持');

-- 2. Chair Pose (Utkatasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('椅子のポーズ (ウトゥカタアーサナ)', 'images/chair_pose.jpeg', 'Yoga', '脚、体幹', '初級',
E'- 膝を曲げて椅子に座るように腰を落とします。\n- 太ももを後ろに引きます。\n- 腕を前か頭上に伸ばします。',
'体重をかかとに乗せ、胸を高く保ちます。',
'太もも、足首、体幹の強化、腹部の引き締め。',
'30秒間保持');

-- 3. Tree Pose (Vrksasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('木のポーズ (ヴルクシャーサナ)', 'images/tree_pose.jpeg', 'Yoga', 'バランス、脚', '初級',
E'- 片足立ちになります。\n- もう片方の足の裏を内もも、ふくらはぎ、または足首に置きます。\n- 手のひらを胸の前で合わせるか、頭上に伸ばします。',
'一点を見つめ、呼吸を深くすることでバランスを保ちます。',
'バランス感覚の向上、集中力強化、脚の筋力アップ。',
'30秒間保持（左右）');

-- 4. Warrior I (Virabhadrasana I)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('戦士のポーズI (ヴィーラバドラーサナI)', 'images/warrior_i.jpeg', 'Yoga', '脚、体幹', '初級',
E'- 片足を前に踏み出し、膝を曲げます。\n- 後ろ足はまっすぐ伸ばし、つま先を少し外側に向けます。\n- 腕を頭上に上げ、手のひらを向かい合わせます。',
'腰をマットの正面に向け、前の膝が足首の真上に来るようにします。',
'脚と体幹の強化、股関節と胸を開く、バランス感覚の向上。',
'30秒間保持（左右）');

-- 5. Warrior II (Virabhadrasana II)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('戦士のポーズII (ヴィーラバドラーサナII)', 'images/warrior_ii.jpeg', 'Yoga', '脚、体幹', '初級',
E'- 片足を前に踏み出し、膝を90度に曲げます。\n- 後ろ足はまっすぐ伸ばし、足はマットの後ろと平行にします。\n- 腕を肩の高さで横に広げ、視線は前の手の指先へ。',
'前の膝が足首の真上に来るようにし、腰を横に開きます。',
'脚と体幹の強化、股関節と胸を開く、スタミナ向上。',
'30秒間保持（左右）');

-- 6. Triangle Pose (Trikonasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('三角のポーズ (トリコーナサナ)', 'images/triangle_pose.jpeg', 'Yoga', '体側、脚', '初級',
E'- 足を大きく開いて立ちます。\n- 片方のつま先を90度外側に向け、もう片方は少し内側に向けます。\n- 体幹を前に伸ばし、股関節から体を倒します。\n- 片手をすね、足首、または床に置き、もう片方の腕を天井に伸ばします。',
'両足を強くまっすぐに保ち、体が2枚のガラス板の間にあるように意識します。',
'股関節、鼠径部、ハムストリング、ふくらはぎのストレッチ、脚と体幹の強化。',
'30秒間保持（左右）');

-- 7. Goddess Pose (Utkata Konasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('女神のポーズ (ウトゥカタ・コーナアーサナ)', 'images/goddess_pose.jpeg', 'Yoga', '脚、股関節', '初級',
E'- 足を大きく開いて立ち、つま先を外側に向けます。\n- 膝を曲げてワイドスクワットの姿勢になり、太ももを床と平行に保ちます。\n- 腕を横か頭上に上げ、肘を曲げ、手のひらを前に向けます。',
'背骨を長く保ち、体幹を意識し、膝がつま先と同じ方向を向くようにします。',
'脚と体幹の強化、股関節と鼠径部を開く、体内の熱を高める。',
'30秒間保持');

-- 8. Prayer Pose (Anjali Mudra / Pranamasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('合掌のポーズ (アンジャリ・ムドラ / プラナマーサナ)', 'images/prayer_pose.jpeg', 'Yoga', '全身', '初級',
E'- 足を揃えてまっすぐ立ちます。\n- 手のひらを胸の前で合わせ、肘を横に広げます。\n- 呼吸に集中し、心を落ち着かせます。',
'手のひらをしっかりと押し合わせ、肩をリラックスさせます。',
'集中力とバランスの向上、心の落ち着き、感謝の気持ちを育む。',
'30秒間保持');

-- 9. Boat Pose (Navasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('舟のポーズ (ナーヴァーサナ)', 'images/boat_pose.jpeg', 'Yoga', '体幹、股関節', '初級',
E'- 床に座り、膝を曲げ、足を床に置きます。\n- 少し後ろに傾き、足を床から持ち上げます。\n- 脚をまっすぐ伸ばすか（修正のために膝を曲げても良い）、腕を床と平行に前に伸ばします。',
'背骨を長く保ち、胸を高く上げ、体幹を意識して背中が丸まらないようにします。',
'体幹、股関節屈筋、背骨の強化、バランス感覚の向上。',
'15-30秒間保持');

-- 10. Bridge Pose (Setu Bandhasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('橋のポーズ (セツバンダーサナ)', 'images/bridge_pose.jpeg', 'Yoga', '背中、お尻、ハムストリング', '初級',
E'- 仰向けになり、膝を曲げ、足を床に腰幅に開いて置きます。\n- 足で床を押し、腰を床から持ち上げます。\n- 手を背中の下で組むか、体の横に置きます。',
'太ももを平行に保ち、膝が開かないようにします。腰だけでなく、股関節から持ち上げるように意識します。',
'背中、お尻、ハムストリングの強化、胸と背骨のストレッチ。',
'30秒間保持');
