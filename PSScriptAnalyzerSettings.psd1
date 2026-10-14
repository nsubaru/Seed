@{
    IncludeRules = @('PSPlaceOpenBrace', 'PSPlaceCloseBrace', 'PSUseConsistentIndentation')

    Rules =
    @{
        PSPlaceOpenBrace =
        @{
            Enable = $true
            OnSameLine = $false
            NewLineAfter = $true
            IgnoreOneLineBlock = $false
        }

        PSPlaceCloseBrace =
        @{
            Enable = $true
            NewLineAfter = $true
            NoEmptyLineBefore = $true
            IgnoreOneLineBlock = $false
        }

        PSUseConsistentIndentation =
        @{
            Enable = $true
            Kind = 'space'
            IndentationSize = 4
            PipelineIndentation = 'IncreaseIndentationForFirstPipeline'
        }
    }
}
