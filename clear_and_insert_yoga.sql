-- Disable foreign key checks for data clearing
SET session_replication_role = 'replica';

-- Clear data from tables, except 'users'
TRUNCATE TABLE exercise_history RESTART IDENTITY CASCADE;
TRUNCATE TABLE user_sessions RESTART IDENTITY CASCADE;
TRUNCATE TABLE user_profiles RESTART IDENTITY CASCADE;
TRUNCATE TABLE exercises RESTART IDENTITY CASCADE;

-- Re-enable foreign key checks
SET session_replication_role = 'origin';

-- Insert 10 easy front-facing yoga poses

-- 1. Mountain Pose (Tadasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('山のポーズ (ターダーサナ)', 'images/mountain_pose.png', 'Yoga', '全身', '初級',
E'- 足を揃えて立ちます。\n- 手のひらを体の横で前向きにします。\n- 背筋を伸ばし、肩をリラックスさせます。\n- 呼吸に集中します。',
'足の裏全体で均等に体重を支え、体の軸を感じます。',
'姿勢の改善、バランス感覚の向上、心の落ち着き。',
'30秒間保持');

-- 2. Child''s Pose (Balasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('子供のポーズ (バーラーサナ)', 'images/child_pose.png', 'Yoga', '背中、腰', '初級',
E'- 膝立ちになります。\n- お尻をかかとに乗せます。\n- 上半身を前に倒し、額を床につけます。\n- 腕を体の横に伸ばします。',
'必要であれば膝を広げ、お腹のスペースを作ります。',
'背中と首のストレッチ、リラックス効果、ストレス軽減。',
'1分間保持');

-- 3. Cat-Cow Pose (Marjaryasana-Bitilasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('猫と牛のポーズ (マルジャーリャーサナ・ビティラーサナ)', 'images/cat_cow_pose.png', 'Yoga', '背骨、腹部', '初級',
E'- 四つん這いになります。\n- 息を吸いながら背中を反らせて顔を上げます（牛のポーズ）。\n- 息を吐きながら背中を丸めておへそを見ます（猫のポーズ）。',
'呼吸と動きを連動させ、背骨の柔軟性を高めます。',
'背骨の柔軟性向上、腹部のマッサージ、ストレス軽減。',
'5-10回繰り返す');

-- 4. Downward-Facing Dog (Adho Mukha Svanasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('下向きの犬のポーズ (アドー・ムカ・シュヴァナーサナ)', 'images/downward_dog.png', 'Yoga', '全身', '初級',
E'- 四つん這いから、お尻を高く持ち上げます。\n- 体を逆V字にします。\n- 手と足で床を押し、背筋を伸ばします。',
'かかとが床につかなくても大丈夫です。膝を軽く曲げても良いです。',
'全身のストレッチ、筋力強化、疲労回復。',
'30秒間保持');

-- 5. Warrior II (Virabhadrasana II)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('戦士のポーズII (ヴィーラバドラーサナII)', 'images/warrior_ii.png', 'Yoga', '脚、体幹', '初級',
E'- 足を大きく開きます。\n- 片方のつま先を前に向け、もう片方のつま先を横に向けます。\n- 前の膝を90度に曲げます。\n- 腕を肩の高さに広げます。',
'前の膝が足首の真上に来るようにし、骨盤を横に向けます。',
'脚と体幹の強化、集中力向上、自信を高める。',
'30秒間保持（左右）');

-- 6. Triangle Pose (Trikonasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('三角のポーズ (トリコーナサナ)', 'images/triangle_pose.png', 'Yoga', '体側、脚', '初級',
E'- 足を大きく開きます。\n- 片方のつま先を前に向け、もう片方のつま先を横に向けます。\n- 上半身を横に倒します。\n- 下の手をすねか床に、上の手を天井に伸ばします。',
'体側を長く保ち、胸を開くように意識します。',
'体側のストレッチ、脚の強化、消化促進。',
'30秒間保持（左右）');

-- 7. Tree Pose (Vrksasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('木のポーズ (ヴルクシャーサナ)', 'images/tree_pose.png', 'Yoga', 'バランス、脚', '初級',
E'- 片足立ちになります。\n- もう片方の足の裏を内もも、ふくらはぎ、または足首に置きます。\n- 手のひらを胸の前で合わせるか、頭上に伸ばします。',
'一点を見つめ、呼吸を深くすることでバランスを保ちます。',
'バランス感覚の向上、集中力強化、脚の筋力アップ。',
'30秒間保持（左右）');

-- 8. Cobra Pose (Bhujangasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('コブラのポーズ (ブジャンガーサナ)', 'images/cobra_pose.png', 'Yoga', '背中、腹部', '初級',
E'- うつ伏せになります。\n- 手のひらを肩の下に置きます。\n- 息を吸いながら上半身をゆっくりと持ち上げ、胸を開きます。',
'肩をリラックスさせ、首を長く保ちます。腰に負担をかけないように注意します。',
'背骨の柔軟性向上、腹部の引き締め、消化促進。',
'15-30秒間保持');

-- 9. Bridge Pose (Setu Bandhasana)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('橋のポーズ (セツバンダーサナ)', 'images/bridge_pose.png', 'Yoga', '背中、お尻、ハムストリング', '初級',
E'- 仰向けになります。\n- 膝を立ててかかとをお尻に近づけます。\n- 息を吸いながらお尻を持ち上げ、胸を顎に近づけます。',
'太ももが平行になるように意識し、膝が外に開かないようにします。',
'背中とお尻の強化、胸を開く、ストレス軽減。',
'30秒間保持');

-- 10. Savasana (Corpse Pose)
INSERT INTO exercises (name, image_path, category, primary_muscle, difficulty_level, instructions, tips, benefits, reps_sets_suggestion) VALUES
('シャヴァーサナ (屍のポーズ)', 'images/savasana.png', 'Yoga', '全身', '初級',
E'- 仰向けになります。\n- 手足を体の横に広げ、手のひらを上向きにします。\n- 目を閉じ、全身の力を抜き、呼吸に集中します。',
'完全にリラックスし、体の感覚や思考を手放します。',
'心身のリラックス、ストレス解消、疲労回復。',
'5分間保持');
