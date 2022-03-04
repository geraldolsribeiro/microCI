#!/bin/bash -x

# shellcheck disable=SC2034
MICROCI_STEP_NAME="Aqui vai o nome do passo atual"
MICROCI_STEP_DESCRIPTION="Aqui vai a descrição do passo atual"
MICROCI_STEP_STATUS=":ok:"
MICROCI_STEP_DURATION="63"

MICROCI_GIT_ORIGIN=$( git config --get remote.origin.url || echo "SEM GIT ORIGIN" )
MICROCI_GIT_COMMIT=$( git rev-parse --short HEAD || echo "SEM GIT COMMIT")
MICROCI_GIT_COMMIT_MSG=$( git show -s --format=%s )

MICROCI_DISCORD_WEBHOOK=https://discord.com/api/webhooks/943613047375802470/sG8Esn7vuKfgqywFf2Uc30ISQLQDl__GffglPGQ5nxySNVHwApRiWL6KW8XOcw7NO-Si 

DISCORD_PAYLOAD=$( jq --null-input \
  --arg content       "$MICROCI_STEP_NAME: $MICROCI_STEP_STATUS" \
  --arg title         "$MICROCI_STEP_NAME" \
  --arg description   "$MICROCI_STEP_DESCRIPTION" \
  --arg repo          "$MICROCI_GIT_ORIGIN" \
  --arg commit        "$MICROCI_GIT_COMMIT" \
  --arg commit_msg    "$MICROCI_GIT_COMMIT_MSG" \
  --arg step_status   "$MICROCI_STEP_STATUS" \
  --arg step_duration "$MICROCI_STEP_DURATION" \
  '
  .content = $content                         |
  .embeds[0].title = $title                   |
  .embeds[0].description = $description       |
  .embeds[0].fields[0].name = "Repositório"   |
  .embeds[0].fields[0].value = $repo          |
  .embeds[0].fields[0].inline = false         |
  .embeds[0].fields[1].name = "Commit"        |
  .embeds[0].fields[1].value = $commit        |
  .embeds[0].fields[1].inline = true          |
  .embeds[0].fields[2].name = "Comentário"    |
  .embeds[0].fields[2].value = $commit_msg    |
  .embeds[0].fields[2].inline = true          |
  .embeds[0].fields[3].name = "Status"        |
  .embeds[0].fields[3].value = $step_status   |
  .embeds[0].fields[3].inline = true          |
  .embeds[0].fields[4].name = "Duração"       |
  .embeds[0].fields[4].value = $step_duration |
  .embeds[0].fields[4].inline = true
  '
)

curl \
  -H "Content-Type: application/json" \
  -H "Expect: application/json" \
  --data "$DISCORD_PAYLOAD" \
  -X POST "$MICROCI_DISCORD_WEBHOOK"


# [[ -z "$MICROCI_DISCORD_WEBHOOK" ]] && exit 0

# source notify_discord.sh
#notify_discord


# vim: ft=bash
