#!/bin/bash

for i in 1 2 3
do
  docker run -d --name gitlab-runner-host-${i} --restart always \
    -v /home/rom1/Projects/poker-bot/docker/gitlab-runner/config/host-${i}:/etc/gitlab-runner \
    -v /var/run/docker.sock:/var/run/docker.sock \
    gitlab/gitlab-runner:latest
done
