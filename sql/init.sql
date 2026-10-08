CREATE TABLE IF NOT EXISTS capability (
    id          BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    name        TEXT NOT NULL UNIQUE,
    description TEXT NOT NULL,
    enabled     BOOLEAN NOT NULL DEFAULT TRUE
);

CREATE TABLE IF NOT EXISTS call_log (
    id          BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    request_id  BIGINT NOT NULL,
    command     TEXT NOT NULL,
    thread_id   TEXT,
    started_at  TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    finished_at TIMESTAMPTZ,
    status      TEXT NOT NULL DEFAULT 'started'
);

INSERT INTO capability(name, description) VALUES
    ('process', 'Выполнение команды из белого списка с захватом stdout/exit code')
ON CONFLICT (name) DO UPDATE
SET description = EXCLUDED.description;
