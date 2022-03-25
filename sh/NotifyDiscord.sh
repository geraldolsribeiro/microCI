function notify_discord() {
  {
    if [[ -z "$MICROCI_DISCORD_WEBHOOK" ]]; then
      echo "Erro: MICROCI_DISCORD_WEBHOOK não foi definida"
      return 1
    fi

    DISCORD_PAYLOAD=$( jq --null-input \
      --arg content       "$MICROCI_STEP_NAME: $MICROCI_STEP_STATUS" \
      --arg title         "$MICROCI_STEP_NAME" \
      --arg description   "$MICROCI_STEP_DESCRIPTION" \
      --arg repo          "$MICROCI_GIT_ORIGIN" \
      --arg commit_sha    "$MICROCI_GIT_COMMIT_SHA" \
      --arg commit_msg    "$MICROCI_GIT_COMMIT_MSG" \
      --arg step_status   "$MICROCI_STEP_STATUS" \
      --arg status_color  $MICROCI_STEP_STATUS_COLOR \
      --arg step_duration "$MICROCI_STEP_DURATION" \
      '
      .content = $content                         |
      .embeds[0].title = $title                   |
      .embeds[0].color = $status_color            |
      .embeds[0].description = $description       |
      .embeds[0].fields[0].name = "Repositório"   |
      .embeds[0].fields[0].value = $repo          |
      .embeds[0].fields[0].inline = false         |
      .embeds[0].fields[1].name = "Commit"        |
      .embeds[0].fields[1].value = $commit_sha    |
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

    echo "$DISCORD_PAYLOAD"

    notify_result=$( curl \
      -H "Content-Type: application/json" \
      -H "Expect: application/json" \
      --data "$DISCORD_PAYLOAD" \
      -X POST "$MICROCI_DISCORD_WEBHOOK" 2>/dev/null
    )
    notify_status=$?

    if [[ "${notify_status}" -ne 0 ]]; then
      echo "Erro: curl falhou ao notificar discord com código ${notify_status}"
      return 1
    fi

    echo "Resultado da notificação:"
    echo "${notify_result}" | jq '.'
  } >> .microCI.log
}

# vim: ft=bash
