CREATE TABLE IF NOT EXISTS capability (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    name TEXT NOT NULL UNIQUE,
    description TEXT NOT NULL,
    enabled BOOLEAN NOT NULL DEFAULT TRUE
);

INSERT INTO capability (name, description)
VALUES (
    'file',
    'Safely read text files and return content and file metadata'
)
ON CONFLICT (name)
DO UPDATE SET
    description = EXCLUDED.description,
    enabled = TRUE;

CREATE TABLE IF NOT EXISTS file_worker_calls (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    file_path TEXT NOT NULL,
    file_name TEXT NOT NULL,
    file_size_bytes BIGINT,
    success BOOLEAN NOT NULL,
    error_message TEXT,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE INDEX IF NOT EXISTS idx_file_worker_calls_created_at
ON file_worker_calls (created_at DESC);
