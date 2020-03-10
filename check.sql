create table test (a int, b int, c int);
insert into _last (a, b, c) values (123, 456, 789);
insert into _last (a, b, c) values (10, 11, 12);
select a, b, c from _last;