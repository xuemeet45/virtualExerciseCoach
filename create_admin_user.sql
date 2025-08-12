-- Create admin user with proper password hash
-- Password: admin123
-- SHA-256 hash of "admin123" is: 240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9

INSERT INTO users (username, email, password_hash, first_name, last_name, is_active) 
VALUES ('admin', 'admin@example.com', '240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9', 'Admin', 'User', true)
ON CONFLICT (username) DO UPDATE SET 
    email = EXCLUDED.email,
    password_hash = EXCLUDED.password_hash,
    first_name = EXCLUDED.first_name,
    last_name = EXCLUDED.last_name,
    is_active = EXCLUDED.is_active; 