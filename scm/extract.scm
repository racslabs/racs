;; scm wrapper for command: EXTRACT <stream-id> <from> <to>
(define (extract stream-id from to)
  (let ((cmd (format #f "EXTRACT ~a ~a ~a" stream-id from to)))
    (c_auxts_db_execute cmd)))