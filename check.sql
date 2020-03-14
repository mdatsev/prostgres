create table test (a int, b string, c int);
insert into _last (a, b, c) values (123, 'test', 789);
insert into _last (a, b, c) values (10, 'testing', 12);
select a, b, c from _last;