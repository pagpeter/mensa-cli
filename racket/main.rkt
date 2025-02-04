#lang racket
(require net/http-client net/url json)

(define-values (status headers body-port) 
  (http-sendrecv "mensa.aaronschlitt.de" "/meals/Griebnitzsee?lang=de&evening=false&date=2025-02-04" #:ssl? #t))

(define body (port->bytes body-port))
(define json (bytes->string/utf-8 body))
(define data (string->jsexpr json))

(displayln status)

(define (title-length meal) (string-length (hash-ref meal 'name)))

(define max-title-len (apply max (map title-length data)))

(define (print-meal meal) 
    (display "| ")
    (display (hash-ref meal 'name))
    (display (make-string (- max-title-len (string-length (hash-ref meal 'name))) #\ ) )  
    (display " | ")
    (display (hash-ref meal 'studentPrice))
    (display "€")
    (display (make-string (- 5(string-length (number->string (hash-ref meal 'studentPrice)))) #\ ) )
    (display "|\n|")
    (display (make-string (+ max-title-len 10) #\-) )
    (display "|\n")
)



(display "|")
(display (make-string (+ max-title-len 10) #\-) )
(display "|\n")
(for-each print-meal data)