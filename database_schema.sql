-- Users table for authentication
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    first_name VARCHAR(50),
    last_name VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE
);

-- JWT tokens table for session management
CREATE TABLE IF NOT EXISTS user_sessions (
    id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,
    token_hash VARCHAR(255) NOT NULL,
    expires_at TIMESTAMP NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    is_valid BOOLEAN DEFAULT TRUE
);

-- User profile table for additional user information
CREATE TABLE IF NOT EXISTS user_profiles (
    id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,
    age INTEGER,
    height_cm INTEGER,
    weight_kg DECIMAL(5,2),
    fitness_level VARCHAR(20) DEFAULT 'beginner',
    goals TEXT,
    preferences TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Exercises table
CREATE TABLE IF NOT EXISTS exercises (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    image_path VARCHAR(255),
    category VARCHAR(100),
    primary_muscle VARCHAR(100),
    secondary_muscles TEXT,
    equipment TEXT,
    difficulty_level VARCHAR(50),
    instructions TEXT,
    tips TEXT,
    video_url VARCHAR(255),
    reps_sets_suggestion VARCHAR(255),
    benefits TEXT,
    common_mistakes TEXT,
    variations TEXT,
    calories_burned_estimate VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Exercise history table to track user's exercise sessions
CREATE TABLE IF NOT EXISTS exercise_history (
    id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,
    exercise_id INTEGER REFERENCES exercises(id) ON DELETE CASCADE,
    session_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    duration_minutes INTEGER,
    calories_burned INTEGER,
    notes TEXT,
    status VARCHAR(50) DEFAULT '確認', -- New column for exercise status (e.g., '確認', '試行', '実行')
    performed_seconds INTEGER -- New column for actual performed time in seconds
);

-- Create indexes for better performance
CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);
CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
CREATE INDEX IF NOT EXISTS idx_user_sessions_user_id ON user_sessions(user_id);
CREATE INDEX IF NOT EXISTS idx_user_sessions_token_hash ON user_sessions(token_hash);
CREATE INDEX IF NOT EXISTS idx_exercise_history_user_id ON exercise_history(user_id);
CREATE INDEX IF NOT EXISTS idx_exercise_history_exercise_id ON exercise_history(exercise_id);

-- Insert a default admin user (password: admin123)
-- SHA-256 hash of "admin123" is: 240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9
INSERT INTO users (username, email, password_hash, first_name, last_name, is_active) 
VALUES ('admin', 'admin@example.com', '240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9', 'Admin', 'User', true)
ON CONFLICT (username) DO NOTHING;
