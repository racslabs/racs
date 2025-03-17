;; extract
;; SCM wrapper for command: EXTRACT <stream-id> <from> <to>
;; stream-id: The ID of the audio stream (string).
;; from: RFC3339 timestamp indicating start of selected range (string).
;; to: RFC3339 timestamp indicating end of selected range (string).
(define (extract stream-id from to)
  (if (not (and (string? stream-id) (string? from) (string? to)))
    (error "extract: stream-id, from, and to must all be strings.")
    (let ((cmd (format #f "EXTRACT ~a ~a ~a" stream-id from to)))
      (c_auxts_db_execute cmd))))